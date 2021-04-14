#pragma once
#include "encrypt.h"
#include "commons.h"
#include "uid.h"
using namespace std;










//metadata structure definitions
enum userType {
    guest, worker, admin
};
enum operationType {
    add, remove, query, login
};

struct record;
struct userInfo;
struct stockAttr;
struct stockCategory;
struct stockItem;
struct loginToken;
class stockMan;












struct record :public data {
    time_t record_date;
    operationType operation;
    uid userid;
    uid parameter;

    bool write(fstream) override;
    bool load(fstream) override;
};

struct userInfo :public data {
    uid userid;
    string username;
    string password;
    string description;
    time_t regdate;

    userType privilege;

    bool write(fstream) override;
    bool load(fstream) override;
};

struct stockAttr :public data {
    uid attr_id;
    string attr_name;
    string attr_desc;

    bool write(fstream) override;
    bool load(fstream) override;
};

struct stockCategory :public data {
    uid type_id;
    string type_name;
    string type_desc;
    unsigned long long int durance;

    bool write(fstream) override;
    bool load(fstream) override;
};

struct stockItem :public data {
    uid id;
    vector<stockCategory> types;
    vector<stockAttr> attrs;
    unsigned long long int count;
    vector<unsigned long long int> durances;

    bool write(fstream) override;
    bool load(fstream) override;
};

struct loginToken :public data {
    bool valid;
    time_t login_time;
    time_t logout_time;
    userType usertype;
    long userid;

    bool write(fstream) override;
    bool load(fstream) override;
};


class stockMan {
public:
    loginToken login(string username, string password);
    vector<stockCategory>& getCategories(void);
    vector<stockAttr>& getAttributes(void);
    bool addCategory(stockCategory type);
    bool addAttr(stockAttr attr);
private:
};



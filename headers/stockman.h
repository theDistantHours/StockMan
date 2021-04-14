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
    ADD, REMOVE, QUERY, LOGIN
};

struct record;
struct userInfo;
struct stockAttr;
struct stockCategory;
struct stockItem;
struct loginToken;
struct queryResultItems;
struct queryOptions;
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

    unsigned long long int min_count;
    unsigned long long int max_count;

    unsigned long long int count;
    map<int, int> durances;
    map<int, int> dates;

    bool write(fstream) override;
    bool load(fstream) override;
    void sort();
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

struct queryOptions {

};


class stockMan {
public:

    loginToken login(string username, string password);
    bool regist(string username, string password);


    vector<stockCategory>& getCategories(void);
    vector<stockAttr>& getAttributes(void);
    vector<uid> query(queryOptions options);

    bool addCategory(stockCategory type,loginToken token, string comment = "No comment");
    bool addAttr(stockAttr attr, loginToken token, string comment = "No comment");
    bool removeCategory(uid id, loginToken token, string comment = "No comment");
    bool removeCategory(string name, loginToken token, string comment = "No comment");
    bool removeAttr(uid id, loginToken token, string comment = "No comment");
    bool removeAttr(string name, loginToken token, string comment = "No comment");

    bool InStock(string name, unsigned long long int count, unsigned long long int durance, loginToken token,string comment ="No comment");
    bool InStock(uid id, unsigned long long int count, unsigned long long int durance, loginToken token,string comment="No comment");

    bool OutStock(string name, unsigned long long int count, unsigned long long int durance, loginToken token, string comment = "No comment");
    bool OutStock(uid id, unsigned long long int count, unsigned long long int durance, loginToken token, string comment = "No comment");

private:
};



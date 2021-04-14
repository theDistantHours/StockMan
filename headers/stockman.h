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
enum operationTarget {
    stock,category,attr,users
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
class logMan;
class statMan;


struct record :public data {
    time_t record_date;
    operationType operation;
    uid userid;
    uid parameter;

    result write(fstream) override;
    result load(fstream) override;
};

struct userInfo :public data {
    uid userid;
    string username;
    string password;
    string description;
    time_t regdate;

    userType privilege;

    result write(fstream) override;
    result load(fstream) override;
};

struct stockAttr :public data {
    uid attr_id;
    string attr_name;
    string attr_desc;

    result write(fstream) override;
    result load(fstream) override;
};

struct stockCategory :public data {
    uid type_id;
    string type_name;
    string type_desc;
    int durance;

    result write(fstream) override;
    result load(fstream) override;
};

struct stockItem :public data {
    uid id;
    vector<stockCategory> types;
    vector<stockAttr> attrs;
    int price;
    int durance;

    int min_count;
    int max_count;

    int count;
    vector <tuple<time_t,int>> detail;

    result write(fstream) override;
    result load(fstream) override;

};

struct loginToken :public data {
    bool valid;
    time_t login_time;
    time_t logout_time;
    userType usertype;
    long userid;

    result write(fstream) override;
    result load(fstream) override;
};


class statMan :public data{
public:
    friend class stockMan;
    void InStock(uid id, int count, int durance);
    vector<vector<tuple<uid,int,int>>> getStat(tm start,tm end);

    result write(fstream) override;
    result load(fstream) override;
private:
    vector<vector<tuple<uid, int, int>>> stats;
};


class stockMan {
public:
    result loadDatafromFile(string filename);
    result saveData();
    result saveDataAs(string filename);

    loginToken login(string username, string password);
    bool regist(string username, string password);


    vector<stockCategory>& getCategories(void);
    vector<stockAttr>& getAttributes(void);
    string getAttrName(uid id);
    uid getAttrId(string name);
    string getCategoryName(uid id);
    uid getCategoryId(string name);
    string getItemName(uid id);
    uid getItemId(string name);
    
    string getUserInfo(uid id, loginToken token ,string comment = "No comment");

    result addCategory(stockCategory type,loginToken token, string comment = "No comment");
    result addAttr(stockAttr attr, loginToken token, string comment = "No comment");
    result removeCategory(uid id, loginToken token, string comment = "No comment");
    result removeCategory(string name, loginToken token, string comment = "No comment");
    result removeAttr(uid id, loginToken token, string comment = "No comment");
    result removeAttr(string name, loginToken token, string comment = "No comment");

    result removeUser(uid id);
    result removeUser(string name);

    result InStock(string name, int count, int durance, loginToken token,string comment ="No comment");
    result InStock(uid id, int count, int durance, loginToken token,string comment="No comment");


    result log(time_t time, uid userid, operationType opType, operationTarget opTarget, int value);


    statMan m_statMan;

private:
    uid current_logged_user;
    fstream logfile;
};



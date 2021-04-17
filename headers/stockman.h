#pragma once
#include "encrypt.h"
#include "commons.h"
#include "uid.h"
using namespace std;

//metadata structure definitions
enum userType {
    guest, worker, admin
};
enum infoType {
    full, notenough, outdate
};
enum operationType {
    ADD, REMOVE, QUERY, LOGIN
};
enum operationTarget {
    stock, category, attr, users
};

struct record;
struct userInfo;
struct stockAttr;
struct stockCategory;
struct stockItem;
struct loginToken;
struct queryResultItems;
struct queryOptions;
struct stockBrief;
class stockMan;
class logMan;
class statMan;


struct record :public data {
    time_t record_date;
    operationType operation;
    uid userid;
    uid parameter;

    result write(fstream&) override;
    result load(fstream&) override;
};

struct userInfo :public data {
    uid userid;
    string username;
    string password;
    string description;
    time_t regdate;

    userType privilege;

    result write(fstream&) override;
    result load(fstream&) override;
};

struct stockAttr :public data {
    uid id;
    string name;
    string desc;

    result write(fstream&) override;
    result load(fstream&) override;
};

struct stockCategory :public data {
    uid id;
    string name;
    string desc;

    result write(fstream&) override;
    result load(fstream&) override;
};

struct stockItem :public data {
    uid id;
    int price;
    int durance;

    int min_count;
    int max_count;

    int count;

    stockCategory category;
    
    string name;
    string desc;

    vector<uid> attrs;
    vector<tuple<time_t, int>> detail;

    result write(fstream&) override;
    result load(fstream&) override;

};

struct loginToken  {
    bool valid;
    long userid;
    time_t login_time;
    time_t logout_time;
    userType usertype;

};


class stockMan {
public:
    result loadDatafromFile(fstream& fs);
    result saveData();
    result saveDataAs(fstream& filename);

    loginToken login(string username, string password);
    loginToken getCurrentToken(void);
    result regist(string username, string password, userType usrType = guest);

    vector<stockCategory> getCategories(void);
    vector<stockAttr> getAttrs(void);
    vector<stockItem> getItems(uid category, set<uid> attrs);
    vector<userInfo> getUsers(void);

    stockAttr getAttr(string name);
    stockAttr getAttr(uid id);
    stockCategory getCategory(string name);
    stockCategory getCategory(uid id);
    stockItem getItem(uid id);
    stockItem getItem(string name);
    userInfo getUserInfo(uid id);

    result findUser(uid id);
    result findUser(string name);

    result addCategory(stockCategory type, string comment = "No comment");
    result addAttr(stockAttr attr, string comment = "No comment");
    result addItem(stockItem item, string comment = "No comment");

    result editCategory(uid id, string name, string desc);
    result editItem(uid id, string name, string desc,string comment = "No comment");
    result editUser(uid id, string name, string password, userType type, string comment = "No comment");

    result removeCategory(uid id, string comment = "No comment");
    result removeAttr(uid id, string comment = "No comment");
    result removeItem(uid id, string comment = "No comment");
    result removeUser(uid id);

    result InStock(uid id, int count, int durance, string comment = "No comment");
    vector<tuple<infoType, stockCategory, stockItem, int, int>> overview(void);

    stockMan() {
        
    }
    stockMan(string filename) {

    }

private:
    loginToken current_login_token;
    fstream logfile;
    fstream datfile;
    map<uid, vector<stockItem>> data;
    vector<stockCategory> categories;
    vector<stockAttr> attributes;

    result log(time_t time, uid userid, operationType opType, operationTarget opTarget, int value);

};



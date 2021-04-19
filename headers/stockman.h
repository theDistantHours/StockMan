#pragma once
#include "encrypt.h"
#include "commons.h"
#include "uid.h"
using namespace std;

//metadata structure definitions
enum class userType {
    guest, worker, admin
};
enum class infoType {
    full, notenough, outdate
};
enum class operationType {
    ADD, REMOVE, QUERY, LOGIN
};
enum class operationTarget {
    stock, category, attr, users
};
enum class  statrange{ week, month, year };

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

    result write(ofstream&) override;
    result load(ifstream&) override;
};

struct userInfo :public data {
    uid userid = -1;
    string username = "";
    string password = "";
    string description = "";
    time_t regdate = 0;

    userType privilege = userType::guest;

    result write(ofstream&) override;
    result load(ifstream&) override;

    static userInfo empty;
    bool operator==(userInfo& b) {
        if (b.userid == userid && b.username == username && b.password == password && b.description == description && b.regdate == regdate)return true;
        return false;
    }
};

struct stockAttr :public data {
    uid id = -1;
    string name = "";
    string desc = "";

    result write(ofstream&) override;
    result load(ifstream&) override;

    bool operator==(stockAttr& b) {
        if (b.id == id && b.name == name && b.desc == desc)return true;
        return false;
    }
};

struct stockCategory :public data {
    uid id = -1;
    string name = "";
    string desc = "";

    result write(ofstream&) override;
    result load(ifstream&) override;

    bool operator==(stockCategory& b) {
        if (b.id == id && b.name == name && b.desc == desc)return true;
        return false;
    }
};

struct stockItem :public data {
    uid id = -1;
    float price = 0;
    int durance = 0;

    int min_count = 0;
    int max_count = 0;

    int count = 0;
    uid category = -1;
    
    string name = "";
    string desc = "";

    vector<uid> attrs;
    vector<tuple<time_t, int>> detail;

    result write(ofstream&) override;
    result load(ifstream&) override;

    bool operator==(stockItem& b) {
        if (b.id == id && b.name == name && b.desc == desc && b.price == price && b.durance == durance && b.min_count == min_count &&
            b.max_count == max_count && b.category == category && b.attrs == attrs && b.detail == detail)return true;
        return false;
    }
};

struct loginToken  {
    bool valid;
    long userid;
    time_t login_time;
    time_t logout_time;
    userType usertype;

};
class logMan {
public:
    result setLogfile(string filename);
    result setLoginToken(loginToken token_p, string username_p) {
        token = token_p;
        username = username_p;
        return result::success;
    }
    result log(string text);
    logMan() {
        token = loginToken();
        username = "";
    }
    ~logMan() {
        file.close();
    }
private:
    string filename;
    loginToken token;
    string username;
    ofstream file;
};

class stockMan {
public:
    result loadDatafromFile(ifstream& fs);
    result saveData();
    result saveDataAs(ofstream& filename);

    loginToken login(string username, string password);
    loginToken getCurrentToken(void);
    result regist(string username, string password, userType usrType = userType::guest);

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
    result editItem(uid id, string name, string desc,uid cate,string comment = "No comment");
    result editUser(uid id, string name, string password, userType type, string comment = "No comment");

    result removeCategory(uid id, string comment = "No comment");
    result removeAttr(uid id, string comment = "No comment");
    result removeItem(uid id, string comment = "No comment");
    result removeUser(uid id);

    result InStock(uid id, int count, string comment = "No comment");
    vector<tuple<infoType, string, string, float, float>> overview(void);
    tuple<map<int, float>, map<int, float>> getstat(statrange range, tuple<int, int, int> lim);
    map<string, float> getstatCate(statrange range, tuple<int,int,int> lim, bool iscurrency);

    stockMan() {
        datfile_in.open("stock_data.dat",ios::in);
        loadDatafromFile(datfile_in);
        datfile_in.close();
        logfile.open("log_data.dat");
    }
    stockMan(string filename) {
        datfile_in.open(filename);
        loadDatafromFile(datfile_in);
        datfile_in.close();
        logfile.open("log_data.dat");
    }

private:
    loginToken current_login_token;
    ofstream logfile;
    ofstream datfile;
    ifstream datfile_in;
    map<uid, vector<stockItem>> itemdata;
    vector<stockCategory> categories;
    vector<stockAttr> attributes;
    vector<userInfo> userdata;

    logMan logger;
    result log(time_t time, uid userid, operationType opType, operationTarget opTarget, int value);

};



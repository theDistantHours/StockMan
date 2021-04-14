#pragma once
#include "encrypt.h"
#include "commons.h"
#include "uid.h"
using namespace std;


namespace StockMan {
    //metadata structure definitions
    enum userType {
        guest, worker, admin
    };
    enum operationType {
        add,remove,query,login
    };
    struct record;
    struct userInfo;
    struct stockAttr;
    struct stockCategory;
    struct stockItem;
    struct loginToken;
    class stockMan;

    struct record {
        time_t record_date;
        operationType operation;
        uid userid;
        uid parameter;
        string toString();
    };

    struct userInfo {
        uid userid;
        string username;
        string password;
        string description;
        time_t regdate;

        userType privilege;
    };

    struct stockAttr {
        uid attr_id;
        string attr_name;
        string attr_desc;
    };

    struct stockCategory {
        uid type_id;
        string type_name;
        string type_desc;
        unsigned long long int durance;
    };

    struct stockItem {
        uid id;
        vector<stockCategory> types;
        vector<stockAttr> attrs;
        unsigned long long int count;
        vector<unsigned long long int> durances;
    };
    
    struct loginToken {
        bool valid;
        time_t login_time;
        time_t logout_time;
        userType usertype;
        long userid;
    };


    class stockMan {
    public:
        loginToken login(string username, string password);
        vector<stockCategory> &getCategories(void);
        vector<stockAttr>& getAttributes(void);
        bool addCategory(stockCategory type);
        bool addAttr(stockAttr attr);
    private:
    };
    
}

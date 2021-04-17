#include "stockman.h"

string begin(string s) {
    return " <" + s + "> ";
}
string end( string s,bool isin = true) {
    return (isin ? "\n</" :" </")+ s + "> ";
}
string& trim(std::string& s)
{
    if (s.empty())
    {
        return s;
    }

    s.erase(0, s.find_first_not_of(" "));
    s.erase(s.find_last_not_of(" ") + 1);
    return s;
}

string loadline(string type, fstream &file) {
    static string tmp;
    string res;
    file >> tmp;
    if (tmp != begin(type))return "";
    if (!getline(file, res))return "";
    file >> tmp;
    if (tmp != end(type,false))return "";
    return trim(res);
}

result record::write(fstream&)
{
    return result();
}

result record::load(fstream&)
{
    return result();
}

result userInfo::write(fstream &file)
{
    file << begin("userinfo");
    file << userid <<
        begin("username") << username << end("username") << begin("password") << password << end("password") <<
        begin("description") << description << end("description");
    file << regdate;
    switch (this->privilege)
    {
    case guest:
        file << " g ";
        break;
    case worker:
        file << " w ";
        break;
    case admin:
        file << " a ";
        break;
    }
    file << end("userinfo");
    return success;
}

result userInfo::load(fstream &fs)
{
    static string temp;
    fs >> temp;
    if (temp != begin("userinfo"))return file_corrupted;
    fs >> userid;
    username = loadline("username", fs);
    password = loadline("password", fs);
    description = loadline("description", fs);

    if (username == "" || password == "" || description == "")return file_corrupted;
    fs >> this->regdate;
    fs >> temp;
    if (temp == "g")privilege = guest;
    else if (temp == "w")privilege = worker;
    else if (temp == "a")privilege = admin;
    else return file_corrupted;
    fs >> temp;
    if (temp != end("userinfo",false))return file_corrupted;

    return success;
}

result stockAttr::write(fstream &file)
{
    file << begin("stockattr") << id << " " <<
        begin("name") << name << end("name") <<
        begin("desc") << desc << end("desc") <<
        end("stockattr");
    return success;
}

result stockAttr::load(fstream &fs)
{
    string temp;
    fs >> temp;
    if (temp != begin("stockattr"))return file_corrupted;
    name = loadline("name", fs);
    desc = loadline("desc", fs);
    fs >> temp;
    if (temp != end("stockattr", false))return file_corrupted;
    if (name != "" && desc != "")return success;
}

result stockCategory::write(fstream &fs)
{
    fs << begin("stockcategory");
    fs << id <<
        begin("name") << name << end("name") <<
        begin("desc") << desc << end("desc") <<
        end("stockcategory");
    return success;
}

result stockCategory::load(fstream &fs)
{
    string temp;
    fs >> temp;
    if (temp != begin("stockcategory"))return file_corrupted;
    fs >> id;
    name = loadline("name", fs);
    desc = loadline("desc", fs);
    fs >> temp;
    if (temp != end("stockcategory", false))return file_corrupted;
    return success;
}

result stockItem::write(fstream &fs)
{
    if (!fs)return unknown_error;
    uid cate = category.id;
    fs << begin("stockitem") <<
        id << " " << price << " " << durance << " " << min_count << " " << max_count << " " << count << " " << cate <<
        begin("name") << name << end("name") << begin("desc") << desc << end("desc") <<
        begin("attrs");
    fs << attrs.size() << " ";
    for (int i = 0; i < attrs.size(); i++) {
        fs << attrs[i] << " ";
    }
    fs << end("attrs");
    fs << begin("detail");
    fs << detail.size() << " ";
    for (int i = 0; i < detail.size(); i++) {
        fs << get<0>(detail[i]) << " " << get<1>(detail[i]) << " ";
    }
    fs << end("detail");
    fs << end("stockitem");
    return success;
}

result stockItem::load(fstream &fs)
{
    if (!fs)return unknown_error;
    static string tmp;
    static uid cate;
    static int sz;
    fs >> tmp;
    if (tmp != begin("stockitem"))return file_corrupted;
    fs >> id >> price >> durance >> min_count >> max_count >> count >> cate;
    name = loadline("name", fs);
    desc = loadline("desc", fs);
    fs >> tmp; if (tmp != begin("attrs"))return file_corrupted;
    fs >> sz;
    if (sz == 0)return file_corrupted;
    for (int i = 0; i < sz; i++) {
        static uid c;
        fs >> c;
        attrs.push_back( c);
    }
    fs >> tmp; if (tmp != end("attrs"))return file_corrupted;
    fs >> sz; if (sz == 0)return file_corrupted;
    for (int i = 0; i < sz; i++) {
        static time_t t;
        static int c;
        fs >> t >> c;
        detail.push_back(tuple<time_t, int>(t, c));
    }
    fs >> tmp;
    if (tmp != end("detail"))return file_corrupted;
    fs >> tmp; if (tmp != end("stockitem"))return file_corrupted;
    return success;
}


result stockMan::saveData()
{
    if (!datfile.is_open())return file_corrupted;
    datfile << begin("stockman_data");
    {
        datfile << begin("category_definition");
        datfile << categories.size() << " ";
        for (int i = 0; i < categories.size(); i++) {
            categories[i].write(datfile);
        }
        datfile << end("category_definition");
        datfile << begin("attr_definition");
        datfile << attributes.size() << " ";
        for (int i = 0; i < attributes.size(); i++) {
            attributes[i].write(datfile);
        }
        datfile << end("attr_definition");
        datfile << begin("item_data_outer");
        datfile << data.size();
        for (auto i = data.begin(); i != data.end(); i++) {
            datfile << begin("item_data_innner");
            datfile << (*i).first << " ";
            auto vec = (*i).second;
            datfile << vec.size() << " ";
            for (int k = 0; k < vec.size(); k++) {
                vec[k].write(datfile);
            }
            datfile << end("item_data_innner");
        }
        datfile << end("item_data_outer");
    }
    datfile << end("stockman_data");
}

result stockMan::loadDatafromFile(fstream &fs)
{
    if (!fs.is_open())return unknown_error;
    string temp;

    fs >> temp;
    if (temp != begin("stockman_data"))return file_corrupted;
    {
        fs >> temp; if (temp != begin("category_definition"))return file_corrupted;
        int catsize=0, attsize=0, itesize=0;
        fs >> catsize; if (catsize == 0)return file_corrupted;
        for (int i = 0; i < catsize; i++) {
            stockCategory cate_t;
            if(cate_t.load(fs)!=success)return file_corrupted;
            categories.push_back(cate_t);
        }
        fs >> temp;if(temp !=end("category_definition", false))return file_corrupted;
        fs >> temp;if(temp !=begin("attr_definition"))return file_corrupted;
        fs >> attsize; if (attsize == 0)return file_corrupted;
        for (int i = 0; i < attsize; i++) {
            stockAttr att_t;
            if (att_t.load(fs) != success)return file_corrupted;
            attributes.push_back(att_t);
        }
        fs >> temp;if (temp != end("attr_definition", false))return file_corrupted;
        fs >> temp; if (temp != begin("item_data_outer"))return file_corrupted;
        fs >> itesize; if (itesize == 0)return file_corrupted;
        for (int i = 0; i < itesize; i++) {
            fs >> temp; if (temp != begin("item_data_inner"))return file_corrupted;
            int vecsize = 0;
            uid cate_id = 0;
            fs >> cate_id;
            fs >> vecsize; if (vecsize == 0)return file_corrupted;
            for (int i = 0; i < vecsize; i ++ ) {
                stockItem item_t;
                if (item_t.load(fs) != success)return file_corrupted;
                data[cate_id].push_back(item_t);
            }
            fs >> temp; if (temp != end("item_data_inner", false))return file_corrupted;
        }
        fs >> temp; if (temp != end("item_data_outer",false))return file_corrupted;
    }
    fs >> temp; if (temp != end("stockman_data",false))return file_corrupted;
    return success;
}

result stockMan::saveDataAs(fstream &fs)
{
    if (!fs.is_open())return file_corrupted;
    fs << begin("stockman_data");
    {
        fs << begin("category_definition");
        fs << categories.size() << " ";
        for (int i = 0; i < categories.size(); i++) {
            categories[i].write(fs);
        }
        fs << end("category_definition");
        fs << begin("attr_definition");
        fs << attributes.size() << " ";
        for (int i = 0; i < attributes.size(); i++) {
            attributes[i].write(fs);
        }
        fs << end("attr_definition");
        fs << begin("item_data_outer");
        fs << data.size();
        for (auto i = data.begin(); i != data.end(); i++) {
            fs << begin("item_data_innner");
            fs << (*i).first << " ";
            auto vec = (*i).second;
            fs << vec.size() << " ";
            for (int k = 0; k < vec.size(); k++) {
                vec[k].write(fs);
            }
            fs << end("item_data_innner");
        }
        fs << end("item_data_outer");
    }
    fs << end("stockman_data");
    return success;
}

loginToken stockMan::login(string username, string password)
{
    return loginToken();
}

loginToken stockMan::getCurrentToken(void)
{
    return loginToken();
}

result stockMan::regist(string username, string password, userType usrType)
{
    return result();
}


vector<stockCategory> stockMan::getCategories(void)
{
    // TODO: insert return statement here
    return vector<stockCategory>();
}

vector<stockAttr> stockMan::getAttrs(void)
{
    return vector<stockAttr>();
    // TODO: insert return statement here
}

vector<userInfo> stockMan::getUsers(void)
{
    return vector<userInfo>();
}

stockAttr stockMan::getAttr(string name)
{
    return stockAttr();
}

stockAttr stockMan::getAttr(uid id)
{
    return stockAttr();
}


stockCategory stockMan::getCategory(string name)
{
    return stockCategory();
}

stockCategory stockMan::getCategory(uid id)
{
    return stockCategory();
}

stockItem stockMan::getItem(uid id)
{
    return stockItem();
}

stockItem stockMan::getItem(string name)
{
    return stockItem();
}

userInfo stockMan::getUserInfo(uid id)
{
    return userInfo();
}

result stockMan::findUser(uid id)
{
    return result();
}

result stockMan::findUser(string name)
{
    return result();
}

result stockMan::addCategory(stockCategory type, string comment)
{
    return result();
}

result stockMan::addAttr(stockAttr attr, string comment)
{
    return result();
}

result stockMan::addItem(stockItem item, string comment)
{
    return result();
}


result stockMan::editCategory(uid id, string name, string desc)
{
    return result();
}

result stockMan::editItem(uid id, string name, string desc, string comment)
{
    return result();
}

result stockMan::editUser(uid id, string name, string password, userType type, string comment)
{
    return result();
}



result stockMan::removeCategory(uid id, string comment)
{
    return result();
}

result stockMan::removeAttr(uid id, string comment)
{
    return result();
}

result stockMan::removeItem(uid id, string comment)
{
    return result();
}

result stockMan::removeUser(uid id)
{
    return result();
}



result stockMan::InStock(uid id, int count, int durance, string comment)
{
    return result();
}

vector<tuple<infoType, stockCategory, stockItem, int, int>> stockMan::overview(void)
{
    return vector<tuple<infoType, stockCategory, stockItem, int, int>>();
}

vector<stockItem> stockMan::getItems(uid category, set<uid> attrs)
{
    return vector<stockItem>();
}

result stockMan::log(time_t time, uid userid, operationType opType, operationTarget opTarget, int value)
{
    return result();
}

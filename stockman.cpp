#include "stockman.h"

string Begin(string s, bool isin = false)
{
    if (!isin)
        return " <" + s + "> ";
    else
        return "<" + s + ">";
}
string End(string s, bool isin = false)
{
    if (isin)
        return "</" + s + ">";
    else
        return "\n</" + s + "> ";
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

string loadline(string type, ifstream& file)
{
    static string tmp;
    string res;
    file >> tmp;
    if (tmp != Begin(type, true))
        return "";
    if (!getline(file, res))
        return "";
    file >> tmp;
    if (tmp != End(type, true))
        return "";
    return trim(res);
}

result record::write(ofstream&)
{
    return result();
}

result record::load(ifstream&)
{
    return result();
}

result userInfo::write(ofstream& file)
{
    file << Begin("userinfo");
    file << userid << Begin("username") << username << End("username") << Begin("password") << password << End("password") << Begin("description") << description << End("description");
    file << regdate;
    switch (this->privilege)
    {
    case userType::guest:
        file << " g ";
        break;
    case userType::worker:
        file << " w ";
        break;
    case userType::admin:
        file << " a ";
        break;
    }
    file << End("userinfo");
    return result::success;
}

result userInfo::load(ifstream& fs)
{
    static string temp;
    fs >> temp;
    if (temp != Begin("userinfo", true))
        return result::file_corrupted;
    fs >> userid;
    username = loadline("username", fs);
    password = loadline("password", fs);
    description = loadline("description", fs);

    if (username == "")
        return result::file_corrupted;
    fs >> this->regdate;
    fs >> temp;
    if (temp == "g")
        privilege = userType::guest;
    else if (temp == "w")
        privilege = userType::worker;
    else if (temp == "a")
        privilege = userType::admin;
    else
        return result::file_corrupted;
    fs >> temp;
    if (temp != End("userinfo", true))
        return result::file_corrupted;

    return result::success;
}

result stockAttr::write(ofstream& file)
{
    file << Begin("stockattr") << id << " " << Begin("name") << name << End("name") << Begin("desc") << desc << End("desc") << End("stockattr");
    return result::success;
}

result stockAttr::load(ifstream& fs)
{
    string temp;
    fs >> temp;
    if (temp != Begin("stockattr", true))
        return result::file_corrupted;
    fs >> id;
    name = loadline("name", fs);
    desc = loadline("desc", fs);
    fs >> temp;
    if (temp != End("stockattr", true))
        return result::file_corrupted;
    if (name != "")
        return result::success;
    return result::file_corrupted;
}

result stockCategory::write(ofstream& fs)
{
    fs << Begin("stockcategory");
    fs << id << Begin("name") << name << End("name") << Begin("desc") << desc << End("desc") << End("stockcategory");
    return result::success;
}

result stockCategory::load(ifstream& fs)
{
    string temp;
    fs >> temp;
    if (temp != Begin("stockcategory", true))
        return result::file_corrupted;
    fs >> id;
    name = loadline("name", fs);
    desc = loadline("desc", fs);
    fs >> temp;
    if (temp != End("stockcategory", true))
        return result::file_corrupted;
    return result::success;
}

result stockItem::write(ofstream& fs)
{
    if (!fs)
        return result::unknown_error;
    fs << Begin("stockitem") << id << " " << price << " " << durance << " " << min_count << " " << max_count << " " << count << " " << category << Begin("name") << name << End("name") << Begin("desc") << desc << End("desc") << Begin("attrs");
    fs << attrs.size() << " ";
    for (int i = 0; i < attrs.size(); i++)
    {
        fs << attrs[i] << " ";
    }
    fs << End("attrs");
    fs << Begin("detail");
    fs << detail.size() << " ";
    for (int i = 0; i < detail.size(); i++)
    {
        fs << get<0>(detail[i]) << " " << get<1>(detail[i]) << " ";
    }
    fs << End("detail");
    fs << End("stockitem");
    return result::success;
}

result stockItem::load(ifstream& fs)
{
    if (!fs)
        return result::unknown_error;
    static string tmp;
    static int sz;
    fs >> tmp;
    if (tmp != Begin("stockitem", true))
        return result::file_corrupted;
    fs >> id >> price >> durance >> min_count >> max_count >> count >> category;
    name = loadline("name", fs);
    desc = loadline("desc", fs);
    fs >> tmp;
    if (tmp != Begin("attrs", true))
        return result::file_corrupted;
    fs >> sz;
    for (int i = 0; i < sz; i++)
    {
        static uid c;
        fs >> c;
        attrs.push_back(c);
    }
    fs >> tmp;
    if (tmp != End("attrs", true))
        return result::file_corrupted;
    fs >> tmp;
    if (tmp != Begin("detail", true))
        return result::file_corrupted;
    fs >> sz;
    for (int i = 0; i < sz; i++)
    {
        static time_t t;
        static int c;
        fs >> t >> c;
        detail.push_back(tuple<time_t, int>(t, c));
    }
    fs >> tmp;
    if (tmp != End("detail", true))
        return result::file_corrupted;
    fs >> tmp;
    if (tmp != End("stockitem", true))
        return result::file_corrupted;
    return result::success;
}

result stockMan::saveData()
{
    datfile.open("stock_data.dat");
    if (!datfile.is_open())
        return result::file_corrupted;
    datfile << Begin("stockman_data");
    {
        datfile << Begin("category_definition");
        datfile << categories.size() << " ";
        for (int i = 0; i < categories.size(); i++)
        {
            categories[i].write(datfile);
        }
        datfile << End("category_definition");
        datfile << Begin("attr_definition");
        datfile << attributes.size() << " ";
        for (int i = 0; i < attributes.size(); i++)
        {
            attributes[i].write(datfile);
        }
        datfile << End("attr_definition");
        datfile << Begin("item_data_outer");
        datfile << itemdata.size();
        for (auto i = itemdata.begin(); i != itemdata.end(); i++)
        {
            datfile << Begin("item_data_inner");
            datfile << (*i).first << " ";
            auto vec = (*i).second;
            datfile << vec.size() << " ";
            for (int k = 0; k < vec.size(); k++)
            {
                vec[k].write(datfile);
            }
            datfile << End("item_data_inner");
        }
        datfile << End("item_data_outer");
    }
    datfile << Begin("users");
    {
        int sz = userdata.size();
        datfile << sz << " ";
        for (int i = 0; i < sz; i++)
        {
            userdata[i].write(datfile);
        }
    }
    datfile << End("users");
    datfile << End("stockman_data");
    datfile.close();
    logger.log("Data saved.");
    return result::success;
}

result stockMan::loadDatafromFile(ifstream& fs)
{
    if (!fs.is_open())
        return result::unknown_error;
    string temp;

    fs >> temp;
    if (temp != Begin("stockman_data", true))
        return result::file_corrupted;
    {
        fs >> temp;
        if (temp != Begin("category_definition", true))
            return result::file_corrupted;
        int catsize = 0, attsize = 0, itesize = 0;
        fs >> catsize;
        for (int i = 0; i < catsize; i++)
        {
            stockCategory cate_t;
            if (cate_t.load(fs) != result::success)
                return result::file_corrupted;
            categories.push_back(cate_t);
        }
        fs >> temp;
        if (temp != End("category_definition", true))
            return result::file_corrupted;
        fs >> temp;
        if (temp != Begin("attr_definition", true))
            return result::file_corrupted;
        fs >> attsize;
        for (int i = 0; i < attsize; i++)
        {
            stockAttr att_t;
            if (att_t.load(fs) != result::success)
                return result::file_corrupted;
            attributes.push_back(att_t);
        }
        fs >> temp;
        if (temp != End("attr_definition", true))
            return result::file_corrupted;
        fs >> temp;
        if (temp != Begin("item_data_outer", true))
            return result::file_corrupted;
        fs >> itesize;
        for (int i = 0; i < itesize; i++)
        {
            fs >> temp;
            if (temp != Begin("item_data_inner", true))
                return result::file_corrupted;
            int vecsize = 0;
            uid cate_id = 0;
            fs >> cate_id;
            fs >> vecsize;
            for (int i = 0; i < vecsize; i++)
            {
                stockItem item_t;
                if (item_t.load(fs) != result::success)
                    return result::file_corrupted;
                itemdata[cate_id].push_back(item_t);
            }
            fs >> temp;
            if (temp != End("item_data_inner", true))
                return result::file_corrupted;
        }
        fs >> temp;
        if (temp != End("item_data_outer", true))
            return result::file_corrupted;
    }
    fs >> temp;
    if (temp != Begin("users", true))
        return result::file_corrupted;
    int szusr;
    fs >> szusr;
    for (int i = 0; i < szusr; i++) {
        userInfo usr;
        if (usr.load(fs) != result::success)return result::file_corrupted;
        userdata.push_back(usr);
    }
    fs >> temp;
    if (temp != End("users", true))return result::file_corrupted;
    fs >> temp;
    if (temp != End("stockman_data", true))
        return result::file_corrupted;
    return result::success;
}

result stockMan::saveDataAs(ofstream& fs)
{
    if (!fs.is_open())
        return result::file_corrupted;
    fs << Begin("stockman_data");
    {
        fs << Begin("category_definition");
        fs << categories.size() << " ";
        for (int i = 0; i < categories.size(); i++)
        {
            categories[i].write(fs);
        }
        fs << End("category_definition");
        fs << Begin("attr_definition");
        fs << attributes.size() << " ";
        for (int i = 0; i < attributes.size(); i++)
        {
            attributes[i].write(fs);
        }
        fs << End("attr_definition");
        fs << Begin("item_data_outer");
        fs << itemdata.size();
        for (auto i = itemdata.begin(); i != itemdata.end(); i++)
        {
            fs << Begin("item_data_inner");
            fs << (*i).first << " ";
            auto vec = (*i).second;
            fs << vec.size() << " ";
            for (int k = 0; k < vec.size(); k++)
            {
                vec[k].write(fs);
            }
            fs << End("item_data_inner");
        }
        fs << End("item_data_outer");
    }
    fs << Begin("users");
    {
        int sz = userdata.size();
        fs << sz << " ";
        for (int i = 0; i < sz; i++)
        {
            userdata[i].write(fs);
        }
    }
    fs << End("users");
    fs << End("stockman_data");
    return result::success;
}

loginToken stockMan::login(string username, string password)
{
    loginToken token;
    auto x = userdata.begin();
    for (; x != userdata.end(); x++)
    {
        if ((*x).username == username)
            break;
    }
    if (x == userdata.end() || (*x).password != password)
    {
        token.valid = false;
        logger.setLoginToken(token, username);
        logger.log("login failed");
        return token;
    }
    token.userid = (*x).userid;
    token.login_time = time(NULL);
    token.usertype = (*x).privilege;
    token.valid = true;
    current_login_token = token;
    logger.setLoginToken(token, username);
    logger.log("successfully logged in.");
    return current_login_token;
}

loginToken stockMan::getCurrentToken(void)
{
    return current_login_token;
}

result stockMan::regist(string username, string password, userType usrType)
{
    auto x = userdata.begin();
    userInfo res;
    for (; x != userdata.end(); x++)
    {
        if ((*x).username == username)
        {
            return result::item_already_exist;
        }
    }
    res.userid = newuid();
    res.username = username;
    res.password = password;
    res.privilege = usrType;
    userdata.push_back(res);
    logger.log("user " + username + " result::successfully registered.");
    return result::success;
}

vector<stockCategory> stockMan::getCategories(void)
{
    return categories;
}

vector<stockAttr> stockMan::getAttrs(void)
{
    return attributes;
}

vector<userInfo> stockMan::getUsers(void)
{
    //if (current_login_token.usertype != admin)return vector<userInfo>();
    return userdata;
}

vector<string> stockMan::getlogs(void)
{
    if(current_login_token.usertype != userType::admin)return vector<string>();
    
    return logger.getlogs();
}


stockAttr stockMan::getAttr(string name)
{
    auto x = attributes.begin();
    while (x != attributes.end())
    {
        if ((*x).name == name)
            return *x;
        x++;
    }
    return stockAttr();
}

stockAttr stockMan::getAttr(uid id)
{
    auto x = attributes.begin();
    while (x != attributes.end())
    {
        if ((*x).id == id)
            return *x;
        x++;
    }
    return stockAttr();
}

stockCategory stockMan::getCategory(string name)
{
    auto x = categories.begin();
    while (x != categories.end())
    {
        if ((*x).name == name)
            return *x;
        x++;
    }
    return stockCategory();
}

stockCategory stockMan::getCategory(uid id)
{
    auto x = categories.begin();
    while (x != categories.end())
    {
        if ((*x).id == id)
            return *x;
        x++;
    }
    return stockCategory();
}

stockItem stockMan::getItem(uid id)
{
    auto x = itemdata.begin();
    while (x != itemdata.end())
    {
        auto y = (*x).second.begin();
        while (y != (*x).second.end())
        {
            if ((*y).id == id)
                return *y;
            y++;
        }
        x++;
    }
    return stockItem();
}

stockItem stockMan::getItem(string name)
{
    auto x = itemdata.begin();
    while (x != itemdata.end())
    {
        auto y = (*x).second.begin();
        while (y != (*x).second.end())
        {
            if ((*y).name == name)
                return *y;
            y++;
        }
        x++;
    }
    return stockItem();
}

userInfo stockMan::getUserInfo(uid id)
{
    userInfo info;
    userInfo res;
    auto x = userdata.begin();
    while (x != userdata.end())
    {
        if ((*x).userid == id)
            info = *x;
        x++;
    }
    res.userid = info.userid;
    res.description = info.description;
    res.username = info.username;
    return res;
}

result stockMan::findUser(uid id)
{
    auto x = userdata.begin();
    while (x != userdata.end())
    {
        if ((*x).userid == id)
            return result::success;
        x++;
    }
    return result::item_not_found;
}

result stockMan::findUser(string name)
{
    auto x = userdata.begin();
    while (x != userdata.end())
    {
        if ((*x).username == name)
            return result::success;
        x++;
    }
    return result::item_not_found;
}

result stockMan::addCategory(stockCategory nm, string comment)
{
    string name = nm.name;
    auto x = categories.begin();
    while (x != categories.end())
    {
        if ((*x).name == nm.name)
            return result::item_already_exist;
        x++;
    }
    nm.id = newuid();
    categories.push_back(nm);
    logger.log("added category:" + nm.name);
    return result::success;
}

result stockMan::addAttr(stockAttr nm, string comment)
{
    string name = nm.name;
    auto x = attributes.begin();
    while (x != attributes.end())
    {
        if ((*x).name == nm.name)
            return result::item_already_exist;
        x++;
    }
    nm.id = newuid();
    attributes.push_back(nm);
    logger.log("added attribute:" + nm.name);
    return result::success;
}

result stockMan::addItem(stockItem item, string comment)
{
    uid id = item.category;
    stockCategory cate = getCategory(item.category);
    auto x = item.attrs.begin();
    while (x != item.attrs.end())
    {
        if (getAttr(*x) == stockAttr())
            return result::item_not_found;
        x++;
    }
    if (cate == stockCategory())
        return result::item_not_found;
    if (!(getItem(item.name) == stockItem()))
        return result::item_already_exist;

    item.id = newuid();
    itemdata[item.category].push_back(item);
    logger.log("add item:" + item.name);
    return result::success;
}

result stockMan::editCategory(uid id, string name, string desc)
{
    if (getCategory(id) == stockCategory())
        return result::item_not_found;
    auto x = categories.begin();
    while (x != categories.end())
    {
        if ((*x).id == id)
            break;
        x++;
    }
    x->desc = desc;
    x->name = name;
    logger.log("edited item:" + x->name);
    return result::success;
}

result stockMan::editItem(uid id, string name, string desc, uid cate, string comment)
{
    if (getItem(id) == stockItem())
        return result::item_not_found;
    auto x = itemdata.begin();
    vector<stockItem>::iterator y;
    while (x != itemdata.end())
    {
        y = (*x).second.begin();
        while (y != (*x).second.end())
        {
            if ((*y).id == id)
                goto finish;
            y++;
        }
        x++;
    }
finish:
    if (y->category == cate)
    {
        y->name = name;
        y->desc = desc;
        logger.log("edited item:" + y->name + "comment :" + comment);
        return result::success;
    }
    else
    {
        y->name = name;
        y->desc = desc;
        y->category = cate;
        logger.log("edited item:" + y->name + "comment :" + comment);
        x->second.push_back(*y);
        x->second.erase(y);
        return result::success;
    }
}

result stockMan::editUser(uid id, string name, string password, userType type, string comment)
{
    return result();
}

result stockMan::removeCategory(uid id, string comment)
{
    if (getCategory(id) == stockCategory())
        return result::item_not_found;
    auto item = categories.begin();
    while (item != categories.end())
    {
        if ((*item).id == id)
            break;
        item++;
    }
    logger.log("erased category:" + item->name);
    auto x = itemdata.begin();
    while (x != itemdata.end())
    {
        if (x->first == id)
            break;
        x++;
    }
    if (x != itemdata.end())
        itemdata.erase(x);
    categories.erase(item);
    return result::success;
}

result stockMan::removeAttr(uid id, string comment)
{
    if (getAttr(id) == stockAttr())
        return result::item_not_found;
    auto item = attributes.begin();
    while (item != attributes.end())
    {
        if ((*item).id == id)
            break;
        item++;
    }
    logger.log("removed attribute:" + item->name + "\tcomment:" + comment);
    auto x = itemdata.begin();
    while (x != itemdata.end())
    {
        auto y = x->second.begin();
        while (y != x->second.end())
        {
            auto z = find(y->attrs.begin(), y->attrs.end(), id);
            if (z != y->attrs.end())
                y->attrs.erase(z);
            y++;
        }
        x++;
    }
    attributes.erase(item);
    return result::success;
}

result stockMan::removeItem(uid id, string comment)
{
    if (getItem(id) == stockItem())
        return result::item_not_found;
    auto x = itemdata.begin();
    while (x != itemdata.end())
    {
        auto y = x->second.begin();
        while (y != x->second.end())
        {
            if (y->id == id)
            {
                logger.log("removed item:" + y->name + "\tcomment:" + comment);
                x->second.erase(y);
                return result::success;
            }
            y++;
        }
        x++;
    }
    return result::success;
}

result stockMan::removeUser(uid id)
{
    if (getUserInfo(id) == userInfo())
        return result::item_not_found;
    auto x = userdata.begin();
    while (x != userdata.end())
    {
        if (x->userid == id)
        {
            logger.log("user removed:" + x->username);
            userdata.erase(x);
            return result::success;
        }
        x++;
    }
    return result::unknown_error;
}

result stockMan::InStock(uid id, int count, string comment)
{
    if (getItem(id) == stockItem())
        return result::item_not_found;
    auto x = itemdata.begin();
    while (x != itemdata.end())
    {
        auto y = x->second.begin();
        while (y != x->second.end())
        {
            if (y->id == id)
            {
                y->count += count;
                if (y->count < 0)
                    y->count = 0;
                y->detail.push_back({ time(NULL), count });
                logger.log("Instock :" + y->name + "count: " + to_string(count));
                return result::success;
            }
            y++;
        }
        x++;
    }
    return result::unknown_error;
}

vector<tuple<infoType, string, string, float, float>> stockMan::overview(void)
{
    vector<tuple<infoType, string, string, float, float>> res;
    auto x = itemdata.begin();
    while (x != itemdata.end())
    {
        auto y = x->second.begin();
        while (y != x->second.end())
        {
            if (y->count >= y->max_count)
            {
                res.push_back({ infoType::full, getCategory(y->category).name, y->name, y->count, y->max_count });
            }
            if (y->count < y->min_count)
            {
                res.push_back({ infoType::notenough, getCategory(y->category).name, y->name, y->count, y->min_count });
            }
            y++;
        }
        x++;
    }
    sort(res.begin(), res.end(), [](tuple<infoType, string, string, float, float> a, tuple<infoType, string, string, float, float> b) {
        if (get<0>(a) == get<0>(b))
        {
            if (get<1>(a) == get<1>(b))
            {
                return get<2>(a) > get<2>(b);
            }
            else
            {
                return get<1>(a) >= get<1>(b);
            }
        }
        else
        {
            return get<0>(a) >= get<0>(b);
        }
        });
    return res;
}

tuple<map<int, float>, map<int, float>> stockMan::getstat(statrange range, tuple<int, int, int> lim)
{
    tuple<map<int, float>, map<int, float>> res;
    static map<statrange, int> cntmap = { {statrange::week, 7}, {statrange::month, 30}, {statrange::year, 12} };
    for (int i = 0; i < cntmap[range]; i++)
    {
        get<0>(res)[i] = 0;
        get<1>(res)[i] = 0;
    }
    auto x = itemdata.begin();
    while (x != itemdata.end())
    {
        auto y = x->second.begin();
        while (y != x->second.end())
        {
            float pc = y->price;
            auto z = y->detail.begin();
            while (z != y->detail.end())
            {
                tm* date = localtime(&get<0>(*z));
                int count = get<1>(*z);
                switch (range)
                {
                case statrange::week:
                    if (date->tm_year == get<0>(lim) && date->tm_mon == get<1>(lim)
                        && date->tm_mday / 4 == get<2>(lim)) {
                        get<0>(res)[date->tm_wday] += count;
                        get<1>(res)[date->tm_wday] += pc;
                    }
                    break;
                case statrange:: month:
                    if (date->tm_year == get<0>(lim) && date->tm_mon == get<1>(lim)) {
                        get<0>(res)[date->tm_mday] += count;
                        get<1>(res)[date->tm_mday] += pc;
                    }
                    break;
                case statrange::year:
                    if (date->tm_year == get<0>(lim)) {
                        get<0>(res)[date->tm_mon] += count;
                        get<1>(res)[date->tm_mon] += pc;
                    }
                    break;
                default:
                    break;
                }
                z++;
            }
            y++;
        }
        x++;
    }
    return res;
}

vector<stockItem> stockMan::getItems(uid category, set<uid> attrs)
{
    vector<stockItem> res;
    auto x = itemdata.begin();
    while (x != itemdata.end())
    {
        auto y = x->second.begin();
        if (x->first == category)
            while (y != x->second.end())
            {
                if (attrs.size() == 0 || ([&y, &attrs]() {
                    auto z = attrs.begin();
                    while (z != attrs.end())
                    {
                        if (find(y->attrs.begin(), y->attrs.end(), *z) == y->attrs.end())
                            return false;
                        z++;
                    }
                    return true;
                    }()))
                {
                    stockItem current;
                    current.name = y->name;
                    current.desc = y->desc;
                    current.count = y->count;
                    current.durance = y->durance;
                    current.max_count = y->max_count;
                    current.min_count = y->min_count;
                    current.price = y->price;
                    current.id = y->id;
                    res.push_back(current);
                }
                    y++;
            }
        x++;
    }
    sort(res.begin(), res.end(), [](stockItem& a, stockItem& b) {
        return a.name > b.name;
        });
    return res;
}

result logMan::setLogfile(string filename)
{
    file.open(filename,ios::app);
    if (file.is_open())
        return result::success;
    return result::unknown_error;
}

result logMan::log(string text)
{
    if (!file.is_open())
        return result::unknown_error;
    file << "time:\t" << time(NULL) << "\t"
        << "userid:\t" << username << "\t"
        << "action:\t" << text << endl;
    return result::success;
}
vector<string> logMan::getlogs(void) {
    ifstream ifs;
    vector<string> res;
    file.close();
    file.clear();
    ifs.open("log_data.dat",ios::in);
    string cl;
    while (!ifs.eof()) {
        getline(ifs, cl);
        res.push_back(cl);
    }
    ifs.close();
    ifs.clear();
    file.open("log_data.dat",ios::app);
    return res;
}
map<string, float> stockMan::getstatCate(statrange range, tuple<int, int, int> lim,bool iscurrency) {
    map<string, float> res;
    auto x = itemdata.begin();
    while (x != itemdata.end())
    {
        auto y = x->second.begin();
        while (y != x->second.end())
        {
            float pc = y->price;
            auto z = y->detail.begin();
            while (z != y->detail.end())
            {
                tm* date = localtime(&get<0>(*z));
                int count = get<1>(*z);
                switch (range)
                {
                case statrange::week:
                    if (date->tm_year == get<0>(lim) && date->tm_mon == get<1>(lim)
                        && date->tm_mday / 4 == get<2>(lim)) {
                        float dat = iscurrency ? get<1>(*z) * pc : get<1>(*z);
                        res[getCategory(x->first).name] += dat;
                    }
                    break;
                case statrange::month:
                    if (date->tm_year == get<0>(lim) && date->tm_mon == get<1>(lim)
                        ) {
                        float dat = iscurrency ? get<1>(*z) * pc : get<1>(*z);
                        res[getCategory(x->first).name] += dat;
                    }
                    break;
                case statrange::year:
                    if (date->tm_year == get<0>(lim)) {
                        float dat = iscurrency ? get<1>(*z) * pc : get<1>(*z);
                        res[getCategory(x->first).name] += dat;
                    }
                    break;
                default:
                    break;
                }
                z++;
            }
            y++;
        }
        x++;
    }
    return res;
}

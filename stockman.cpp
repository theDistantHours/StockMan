#include "stockman.h"

result record::write(fstream)
{
    return result();
}

result record::load(fstream)
{
    return result();
}

result userInfo::write(fstream)
{
    return result();
}

result userInfo::load(fstream)
{
    return result();
}

result stockAttr::write(fstream)
{
    return result();
}

result stockAttr::load(fstream)
{
    return result();
}

result stockCategory::write(fstream)
{
    return result();
}

result stockCategory::load(fstream)
{
    return result();
}

result stockItem::write(fstream)
{
    return result();
}

result stockItem::load(fstream)
{
    return result();
}

result loginToken::write(fstream)
{
    return result();
}

result loginToken::load(fstream)
{
    return result();
}

void statMan::InStock(uid id, int count, int durance)
{
}

vector<vector<tuple<uid, int, int>>> statMan::getStat(tm start, tm end)
{
    return vector<vector<tuple<uid, int, int>>>();
}

result statMan::write(fstream)
{
    return result();
}

result statMan::load(fstream)
{
    return result();
}

result stockMan::loadDatafromFile(string filename)
{
    return result();
}

result stockMan::saveData()
{
    return result();
}

result stockMan::saveDataAs(string filename)
{
    return result();
}

loginToken stockMan::login(string username, string password)
{
    return loginToken();
}

bool stockMan::regist(string username, string password)
{
    return false;
}

vector<stockCategory> stockMan::getCategories(void)
{
    // TODO: insert return statement here
    return vector<stockCategory>();
}

vector<stockAttr> stockMan::getAttributes(void)
{
    return vector<stockAttr>();
    // TODO: insert return statement here
}

string stockMan::getAttrName(uid id)
{
    return string();
}

uid stockMan::getAttrId(string name)
{
    return uid();
}

string stockMan::getCategoryName(uid id)
{
    return string();
}

uid stockMan::getCategoryId(string name)
{
    return uid();
}

string stockMan::getItemName(uid id)
{
    return string();
}

uid stockMan::getItemId(string name)
{
    return uid();
}

string stockMan::getUserInfo(uid id, loginToken token, string comment)
{
    return string();
}

result stockMan::findUser(uid id)
{
    return result();
}

result stockMan::findUser(string name)
{
    return result();
}

result stockMan::addCategory(stockCategory type, loginToken token, string comment)
{
    return result();
}

result stockMan::addAttr(stockAttr attr, loginToken token, string comment)
{
    return result();
}

result stockMan::removeCategory(uid id, loginToken token, string comment)
{
    return result();
}

result stockMan::removeCategory(string name, loginToken token, string comment)
{
    return result();
}

result stockMan::removeAttr(uid id, loginToken token, string comment)
{
    return result();
}

result stockMan::removeAttr(string name, loginToken token, string comment)
{
    return result();
}

result stockMan::removeUser(uid id)
{
    return result();
}

result stockMan::removeUser(string name)
{
    return result();
}

result stockMan::InStock(string name, int count, int durance, loginToken token, string comment)
{
    return result();
}

result stockMan::InStock(uid id, int count, int durance, loginToken token, string comment)
{
    return result();
}

result stockMan::log(time_t time, uid userid, operationType opType, operationTarget opTarget, int value)
{
    return result();
}

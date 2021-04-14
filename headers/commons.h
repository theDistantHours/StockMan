#pragma once
#include<vector>
#include<string>
#include<fstream>
#include<ctime>
#include<map>



enum result {
    success,
    item_not_found, item_already_exist,item_out_of_capacity,
    category_not_found,category_already_exist,
    attr_not_found,attr_already_exist,
    file_not_found,
    bad_privilege,
    user_not_found,user_already_exist,
    bad_password,
    unknown_error
};

time_t getTime(bool isEmulated);
void initTiming();
void setVirtualTime(time_t t);
time_t randomtime(time_t start, time_t end);
time_t getTime(bool isEmulated);

class data {
    virtual result load(std::fstream file);
    virtual result write(std::fstream file);
};

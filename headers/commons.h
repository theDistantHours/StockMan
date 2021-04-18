#pragma once
#include<vector>
#include<string>
#include<fstream>
#include<ctime>
#include<map>
#include<set>
#include<algorithm>


enum result {
    success,
    item_not_found, item_already_exist,item_out_of_capacity,
    bad_privilege,
    bad_password,
    file_corrupted,
    unknown_error
};

time_t getTime(bool isEmulated);
void initTiming();
void setVirtualTime(time_t t);
time_t randomtime(time_t start, time_t end);
time_t getTime(bool isEmulated);

class data {
    virtual result load(std::ifstream &file) {
        return success;
    }
    virtual result write(std::ofstream &file) {
        return success;
    }
};

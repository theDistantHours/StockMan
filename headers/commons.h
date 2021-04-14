#pragma once
#include<vector>
#include<string>
#include<fstream>
#include<ctime>
#include<map>

class data {
    virtual bool load(std::fstream file);
    virtual bool write(std::fstream file);
};

time_t getTime(bool isEmulated);
void initTiming();
void setVirtualTime(time_t t);
time_t randomtime(time_t start, time_t end);
time_t getTime(bool isEmulated);

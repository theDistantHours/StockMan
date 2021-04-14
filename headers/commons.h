#pragma once
#include<vector>
#include<string>
#include<fstream>
#include<ctime>

class data {
    virtual bool load(std::fstream file);
    virtual bool write(std::fstream file);
};

#pragma once

#include "interval.h"
#include <string>
#include <map>

class Storage
{
public:
    Storage(const std::string & path); 
    std::vector<Interval> intervalVector() const;
    void update(Interval && interval);
    void remove(int id);

private:
    void parse();
    void store();
    int newId() const;

private:
    std::map<int, Interval> intervalMap_;
    std::string path_;
};

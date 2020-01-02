#include "storage.h"
#include <fstream>

Storage::Storage(const std::string & path)
:   path_(path)
{
    parse();
}

std::vector<Interval> Storage::intervalVector() const
{
    std::vector<Interval> intervalVector;
    for (const auto & it : intervalMap_)
        intervalVector.push_back(it.second);

    return intervalVector;
}

void Storage::update(Interval && interval)
{
    auto it = intervalMap_.find(interval.id);
    if (it == intervalMap_.end())
    {
        interval.id = newId();
        intervalMap_.insert(std::make_pair(interval.id, interval));
    }
    else
    {
        it->second = interval;
    }

    store();
}

int Storage::newId() const
{
    int id = 1;
    for (const auto & it : intervalMap_)
    {
        if (it.first == id)
            id++;
    }

    return id;
}

void Storage::parse()
{
    intervalMap_.clear();
    std::ifstream ifs(path_);
    if (ifs.good())
    {
        std::string jsonContent((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
        intervalMap_ = IntervalTranslator::mapFromJson(jsonContent);
    }
    else
    {
        throw std::runtime_error("Unable to read storage file");
    }
}

void Storage::store()
{
    std::ofstream file;
    file.open(path_);
    if (!file.good())
        throw std::runtime_error(std::string("Cannot store file: ") + path_);

    std::string jsonContent(IntervalTranslator::toJson(intervalMap_));
    file << jsonContent;
    file.close();
}

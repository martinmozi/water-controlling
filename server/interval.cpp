#include "interval.h"

std::string IntervalTranslator::toJson(const Interval & interval)
{
    return "";
}
std::string IntervalTranslator::toJson(const std::map<int, Interval> & intervalMap)
{
    return "";
}
Interval IntervalTranslator::fromJson(const std::string & json)
{
    Interval interval;
    return interval;
}
std::map<int, Interval> IntervalTranslator::mapFromJson(const std::string & json)
{
    std::map<int, Interval> intervalMap;
    return intervalMap;
}

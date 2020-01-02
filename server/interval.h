#pragma once

#include <ctime>
#include "jsonVariant.hpp"

enum class SensorType
{
    NoType = -1,
    Sensor = 0,
    Valve = 1,
};

enum class GpioStatus
{
	NoType = -1,
	Enabled = 0,
	Disabled = 1,
	ByInterval = 2,
};

enum DayOfWeek : char
{
    Mon = 0b1,
    Tue = 0b10,
    Wed = 0b100,
    Thu = 0b1000,
    Fri = 0b10000,
    Sat = 0b100000,
    Sun = 0b1000000,
};

struct Interval
{
    int64_t id;
    std::string name;
    std::time_t from;
    std::time_t to;
    char days;
    bool active;
    int64_t gpioIndex;
};

namespace IntervalTranslator
{
    std::string toJson(const Interval & interval);
    std::string toJson(const std::map<int, Interval> & intervalMap);
    Interval fromJson(const std::string & json);
    std::map<int, Interval> mapFromJson(const std::string & json);
}

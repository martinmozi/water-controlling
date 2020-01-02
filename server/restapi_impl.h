#pragma once

#include <vector>
#include "interval.h"
#include "gpio_config.h"
#include "water_manager.h"
#include "db/database.h"

class RestApiImpl
{
public:
    RestApiImpl(const std::string & configPath, const std::string & dbPath);
    void setTime(std::time_t time);
    std::time_t getTime() const;
    std::vector<Interval> intervalList() const;
    void addInterval(const Interval & interval, int64_t & id);
    void modifyInterval(const Interval & interval);
    void removeInterval(const int64_t & id);
	GpioStatus getGpioStatus(int index);
	void setGpioStatus(int index, GpioStatus gpioStatus);

private:
    void init();

private:
    GpioConfig gpioConfig_;
    WaterManager waterManager_;
    Db::Connection db_;
};
#pragma once

#include "interval.h"
#include "storage.h"
#include <set>
#include <atomic>
#include <thread>
#include <mutex>

class WaterManager
{
public:
    WaterManager();
    ~WaterManager();
    void setIntervals(const std::vector<Interval> & intervalVector);
	void setGpioStatus(int index, GpioStatus gpioStatus);

private:
    void run();
    void work();

private:
    std::vector<Interval> intervalVector_;
	std::map<int, GpioStatus> gpioStatuses;
    std::mutex mutex_;
    std::thread thread_;
    std::atomic<bool> stop_;
};
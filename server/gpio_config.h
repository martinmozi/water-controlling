#pragma once

#include "interval.h"

class GpioConfig
{
public:
    GpioConfig(const std::string & configPath);
    const std::map<int64_t, std::string>& gpioData() const;

private:
    void init(const std::string & configPath);

private:
    std::map<int64_t, std::string> gpioData_;
};
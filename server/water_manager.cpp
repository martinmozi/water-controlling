#include "water_manager.h"
#include "gpio.h"

WaterManager::WaterManager()
:   stop_(false),
    thread_([this]() { run(); })
{
}

WaterManager::~WaterManager()
{
    stop_ = true;
    thread_.join();
}

void WaterManager::setIntervals(const std::vector<Interval> & intervalVector)
{
    std::lock_guard<std::mutex> lock(mutex_);

    intervalVector_ = intervalVector;
}

void WaterManager::setGpioStatus(int index, GpioStatus gpioStatus)
{
    std::lock_guard<std::mutex> lock(mutex_);

    // add ranges for pins and set firstly these pins as output
	gpioStatuses[index] = gpioStatus;
	switch (gpioStatus)
	{
	case GpioStatus::Enabled:
		Gpio::instance().setPinValue(index, true);
		break;
		
	case GpioStatus::Disabled:
        Gpio::instance().setPinValue(index, false);
		break;
		
	default:
		break;
	}
}

void WaterManager::run()
{
    while (true)
    {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            work();
        }

        if (stop_)
            return;

        std::this_thread::sleep_for(std::chrono::milliseconds(30000));
    }
}

void WaterManager::work()
{
    // algoritmus na zapnutie prislusnych ventilov
	// treba zohladnit aj statusy
    time_t curr_time = time(NULL);
    for (const Interval & interval : intervalVector_)
    {
        //if (interval.)
    }
}

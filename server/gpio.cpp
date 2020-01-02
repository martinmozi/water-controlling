#include "gpio.h"
#include "log.h"
#include <stdexcept>

#ifdef RASPBERRY_PI
    #include <wiringPi.h>
#endif

Gpio::Gpio()
{
#ifdef RASPBERRY_PI
	if (wiringPiSetup() == -1)
	{
		// some log
		throw std::runtime_error("Unable to initialize wiring PI");
	}

	pinMode(0, OUTPUT);
#endif
}

bool Gpio::getPin(int index)
{
#ifdef RASPBERRY_PI
	return (digitalRead(index) == 1);
#endif
	
	return true;
}

void Gpio::setPin(int index, bool on)
{
    int state = (on == true) ? 1 : 0;
    Log::info("Set state of pin %d to %d", index, state);
#ifdef RASPBERRY_PI
	digitalWrite(index, state);
#endif
}
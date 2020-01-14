#include "gpio.h"
#include "log.h"
#include <stdexcept>

#ifdef RASPBERRY_PI
#include <bcm2835.h>
#endif

Gpio& Gpio::instance()
{
    static Gpio g;
    return g;
}

Gpio::Gpio()
{
#ifdef RASPBERRY_PI
    if (!bcm2835_init())
    {
        Log::error("Unable to initialize bcm2835 library");
        exit(-1);
    }
#endif
}

Gpio::~Gpio()
{
#ifdef RASPBERRY_PI
    bcm2835_close();
#endif
}

void Gpio::setPinMode(int index, PinMode pinMode)
{
#ifdef RASPBERRY_PI
    bcm2835_gpio_fsel(index , pinMode == PinMode::In ? BCM2835_GPIO_FSEL_INPT : BCM2835_GPIO_FSEL_OUTP);
#endif
}

bool Gpio::pinValue(int index)
{
#ifdef RASPBERRY_PI
    uint8_t value = bcm2835_gpio_lev(index);
    return (value > 0);
#endif

    return true;
}

void Gpio::setPinValue(int index, bool on)
{
#ifdef RASPBERRY_PI
    bcm2835_gpio_write(index, on ? HIGH : LOW);
    // bcm2835_delay(500);
#endif
}

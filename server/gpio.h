#pragma once

class Gpio
{
public:
    enum PinMode
    {
        In,
        Out,
    };

public:
	static Gpio & instance();
    ~Gpio();

    void setPinMode(int index, PinMode pinMode);
	bool pinValue(int index);
	void setPinValue(int index, bool on);

private:
    Gpio();
    Gpio(const Gpio &) = delete;
    Gpio& operator=(const Gpio &) = delete;
    Gpio(Gpio&&) = delete;
    Gpio& operator=(Gpio&&) = delete;
};
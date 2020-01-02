#pragma once

class Gpio
{
public:
	Gpio();
	bool getPin(int index);
	void setPin(int index, bool on);
};
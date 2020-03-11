#pragma once

#include <cstddef>
#include "I2CHandle.hpp"

struct PCF8591
{
	static const int CHANNEL_COUNT = 4;

	PCF8591(int addr, int bus);
	float analogRead(int channel);
	// float analogWrite(int channel);

	void update();

	private:
	I2CHandle device;
	std::byte buffer[CHANNEL_COUNT];
	std::byte makeCtlByte(unsigned channelNumber, bool autoIncrement, unsigned inputProgramming, bool outputEnable);
};

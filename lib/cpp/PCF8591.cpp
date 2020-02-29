#include "PCF8591.hpp"
#include <stdexcept>
#include <fmt/format.h>
#include <pigpio.h>

PCF8591::PCF8591(int addr, int bus) : device(addr, bus)
{
	if (addr & 1) {
		throw std::runtime_error(fmt::format("PCF8591: invalid address, addr={}\n", addr));
	}

	// either I don't understand this device or it does some magic
	// outputEnable must be true, channel numbering has offset
    std::byte ctlByte = makeCtlByte(0, true, 0, true); // do not change
    int code = i2cWriteDevice(device, (char*) &ctlByte, 1);
    if (code != 0) {
        throw std::runtime_error(fmt::format("PCF8591: failed to send control byte, code={}\n", code));
    }

    std::byte hello {0};
    code = i2cReadDevice(device, (char*) &hello, 1);
    if (code != 1) {
        throw std::runtime_error(fmt::format("PCF8591: failed to read first byte, code={}\n", code));
    }
}

float PCF8591::analogRead(int channel)
{
	if (channel >= CHANNEL_COUNT) {
		throw std::invalid_argument(fmt::format("PCF8591: invalid channel number"));
	}
	return static_cast<float>(buffer[channel]) / 255.0f;
}

void PCF8591::update()
{
	int code = i2cReadDevice(device, (char*) buffer, CHANNEL_COUNT);
	if (code != CHANNEL_COUNT) {
		throw std::runtime_error(fmt::format("PCF8591: failed to read I2C device, code={}\n", code));
	}
}

std::byte PCF8591::makeCtlByte(unsigned channelNumber, bool autoIncrement, unsigned inputProgramming, bool outputEnable)
{
	return std::byte{0}
		| static_cast<std::byte>(channelNumber)
		| static_cast<std::byte>(autoIncrement << 2U)
		| static_cast<std::byte>(0 << 3U) // reserved
		| static_cast<std::byte>(inputProgramming << 4U)
		| static_cast<std::byte>(outputEnable << 6U);
}
#include <pigpio.h>
#include <stdexcept>
#include <fmt/format.h>
#include "I2CHandle.hpp"

I2CHandle::I2CHandle(int addr, int bus)
{
	handle = i2cOpen(bus, addr, 0);
	if (handle < 0) {
		throw std::runtime_error(fmt::format(
			"I2CHandle: failed to open I2C device, addr={}, bus={}, code={}\n", addr, bus, handle));
	}
}

I2CHandle::operator unsigned()
{
	return (unsigned) handle;
}

I2CHandle::~I2CHandle()
{
	int code = i2cClose(handle);
	if (code != 0) {
		fmt::print(stderr, "I2CHandle: error on closing I2C device, handle={}, code={}\n", handle, code);
	}
}

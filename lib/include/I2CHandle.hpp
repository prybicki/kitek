#pragma once

struct I2CHandle
{
	I2CHandle(int addr, int bus);
	operator unsigned();
	~I2CHandle();
private:
	int handle;
};

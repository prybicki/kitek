#pragma once

#include <mutex>
#include <pigpio.h>

struct EncoderTicks {
    EncoderTicks() : forward(0), backward(0), empty(0), timestamp(gpioTick()) {}
	int forward;
	int backward;
	int empty;
	uint32_t timestamp;

	EncoderTicks operator-(const EncoderTicks& other) const;
};

struct Encoder
{
	Encoder(int pinA, int pinB);
	void callback(int pin, int level, uint32_t tick);
	EncoderTicks getTicks();

private:
	int pinA, pinB;
	unsigned levelA, levelB;
	
	std::mutex ticksMutex;
		EncoderTicks ticks;

	int readWait = 0;
	int writeWait = 0;
};

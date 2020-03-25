#include "Encoder.hpp"
#include <pigpio.h>

const int dirFromPrevABCurrAB[] = {
	+0,  // 00 00
	-1,  // 00 01 -> ccw
	+1,  // 00 10 -> cw
	+0,  // 00 11
	+1,  // 01 00 -> cw
	+0,  // 01 01
	+0,  // 01 10
	-1,  // 01 11 -> ccw
	-1,  // 10 00 -> ccw
	+0,  // 10 01
	+0,  // 10 10
	+1,  // 10 11 -> cw
	+0,  // 11 00
	+1,  // 11 01 -> cw
	-1,  // 11 10 -> ccw
	+0,  // 11 11
};

static void callbackTrampoline(int pin, int level, uint32_t tick, void* data)
{
	auto* encoder = reinterpret_cast<Encoder*>(data);
	encoder->callback(pin, level, tick);
}

void Encoder::callback(int pin, int level, uint32_t tick)
{
	std::lock_guard lock(ticksMutex);
	unsigned state = 0;
	
	state |= (levelA << 3U) | (levelB << 2U);
	unsigned* modifiedLevel = (pin == pinA) ? &levelA : &levelB;
	*modifiedLevel = level;
	state |= (levelA << 1U) | levelB;

	int direction = dirFromPrevABCurrAB[state];
	if (direction == 1)
		ticks.forward += 1;
	else if (direction == -1)
		ticks.backward += 1;
	else
		ticks.empty += 1;
	ticks.timestamp = gpioTick();
}

Encoder::Encoder(int pinA, int pinB)
	: pinA(pinA),
	  pinB(pinB)
{
	gpioSetMode(pinA, PI_INPUT);
	gpioSetMode(pinB, PI_INPUT);
	gpioSetAlertFuncEx(pinA, callbackTrampoline, this);
	gpioSetAlertFuncEx(pinB, callbackTrampoline, this);
	levelA = gpioRead(pinA);
	levelB = gpioRead(pinB);
}

EncoderTicks Encoder::getTicks()
{
	std::lock_guard lock(ticksMutex);
	return ticks;
}

EncoderTicks EncoderTicks::operator-(const EncoderTicks& other) const
{
	EncoderTicks out = *this;
	out.forward -= other.forward;
	out.backward -= other.backward;
	out.empty -= other.empty;
	out.timestamp -= other.timestamp;
	return out;
}
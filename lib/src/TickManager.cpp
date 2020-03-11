#include <TickManager.hpp>
#include <cmath>
#include <pigpio.h>
#include <thread>

TickManagerGPIO::TickManagerGPIO(float hz) :
	periodMs(static_cast<int32_t>(std::lround(1e6f/hz))), 
	tickBeginMs(gpioTick())
{}

void TickManagerGPIO::sleepUntilNextTick() {
	int32_t elapsed = static_cast<int32_t>(gpioTick() - tickBeginMs);
	int32_t toSleep = periodMs - elapsed;
	if (toSleep > 0) {
		gpioDelay(static_cast<uint32_t>(toSleep));
	}
	tickBeginMs = gpioTick();
}

TickManagerChrono::TickManagerChrono(float hz) : 
	period(1.0f/hz),
	tickBegin(HighResClock::now())
{}

void TickManagerChrono::sleepUntilNextTick() {
	auto elapsed = HighResClock::now() - tickBegin;
	auto toSleep = period - elapsed;
	
	if (toSleep > toSleep.zero()) {
		std::this_thread::sleep_for(toSleep);
	}
	tickBegin = HighResClock::now();
}
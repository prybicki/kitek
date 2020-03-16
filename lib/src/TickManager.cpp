#include <TickManager.hpp>
#include <cmath>
#include <pigpio.h>
#include <thread>

TickManagerGPIO::TickManagerGPIO(GPIOClock* clock, float hz) :
	clock(clock),
	periodMs(static_cast<int32_t>(std::lround(1e6f/hz))), 
	tickBeginMs(clock->getTick())
{}

void TickManagerGPIO::sleepUntilNextTick() {
	// TODO verify this
	int32_t elapsed = static_cast<int32_t>(clock->getTick() - tickBeginMs);
	int32_t toSleep = periodMs - elapsed;
	if (toSleep > 0) {
		gpioDelay(static_cast<uint32_t>(toSleep));
	}
	tickBeginMs = clock->getTick();
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

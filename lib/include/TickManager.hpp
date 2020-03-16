#pragma once

#include <cstdint>
#include <ClockAmplifier.hpp>
#include <pigpio.h>

struct TickManagerGPIO
{
	TickManagerGPIO(GPIOClock* clock, float hz);
	void sleepUntilNextTick();

private:
	GPIOClock* clock;
	int32_t periodMs;
	uint64_t tickBeginMs;
};

#include <chrono>
struct TickManagerChrono
{
	template<typename Clock>
	using TimePoint = std::chrono::time_point<Clock>;
	template<typename Rep>
	using Duration = std::chrono::duration<Rep>;
	using HighResClock = std::chrono::high_resolution_clock;

	TickManagerChrono(float hz);
	void sleepUntilNextTick();

	Duration<float> period;
	TimePoint<HighResClock> tickBegin;
};

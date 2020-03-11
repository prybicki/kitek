#pragma once

#include <cstdint>

struct TickManagerGPIO
{
	TickManagerGPIO(float hz);
	void sleepUntilNextTick();

private:
	int32_t periodMs;
	uint32_t tickBeginMs;
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

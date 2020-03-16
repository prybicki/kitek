#include <limits>
#include <functional>

template<auto _getTick, typename HighTick, typename Epoch=int>
struct ClockAmplifier
{
	using LowTick = typename std::invoke_result<decltype(_getTick)>::type;
	HighTick getTick() {
		LowTick currentTick = _getTick();
		epoch += (currentTick < lastTick);
		lastTick = currentTick;
		return epoch * static_cast<HighTick>(std::numeric_limits<LowTick>::max()) + currentTick;
	}

	// Based on the lastTick guesses epoch of the given tick and return its High value.
	HighTick liftTick(LowTick tick) {
		return (epoch + getEpochOffset(tick)) * static_cast<HighTick>(std::numeric_limits<LowTick>::max()) + tick;
	}

	int getEpochOffset(LowTick tick) {
		// TODO: Test it thoroughly and try to simplify.

		// tick is in the close past, it's from current epoch
		//                         V    V
		// |                       t    l   |                       t    l   |
		if (tick <= lastTick && lastTick - tick <= std::numeric_limits<LowTick>::max()/2) {
			return 0;
		}
		
		// tick is in the distant past, it's from future epoch
		//                              V       V
		// |   t                        l   |   t                        l   |
		if (tick <= lastTick && lastTick - tick > std::numeric_limits<LowTick>::max()/2) {
			return 1;
		}

		// tick is in the close future, it's from current epoch
		//                        V   V
		// |                      l   t     |                      l   t     |
		if (tick > lastTick && tick - lastTick <= std::numeric_limits<LowTick>::max()/2) {
			return 0;
		}

		// tick is in the distant future, it's from previous epoch
		//                              V       V
		// |   l                        t   |   l                        t   |
		if (tick > lastTick && tick - lastTick >= std::numeric_limits<LowTick>::max()/2) {
			return -1;
		}

		return +42;
	}

private:
	Epoch epoch = 0;
	LowTick lastTick = 0;
};

#include <pigpio.h>
using GPIOClock = ClockAmplifier<gpioTick, uint64_t>;
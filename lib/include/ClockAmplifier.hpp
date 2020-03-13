#include <limits>

template<auto _getTick, typename HighTick, typename Epoch=int>
struct ClockAmplifier
{
	using LowTick = typename std::invoke_result<decltype(_getTick)>::type;
	HighTick getTick() {
		LowTick currentTick = _getTick();
		epoch += (currentTick < lastTick);
		lastTick = currentTick;
		return static_cast<HighTick>(currentTick) + epoch * std::numeric_limits<LowTick>::max();
	}
	Epoch epoch = 0;
	LowTick lastTick = 0;
};

#include <UDPTransceiver.hpp>
#include <Protocol.pb.h>
#include <cmath>

uint8_t getTick() {
	static uint8_t tick = 0;
	return tick++;
}

#include <ClockAmplifier.hpp>
#include <fmt/format.h>
void testClockAmplifier()
{
	ClockAmplifier<getTick, uint64_t> clock;
	while(true) {
		fmt::print("{} {} {}\n", clock.lastTick, clock.epoch, clock.getTick());
	}
}

int main(int argc, char** argv)
{
	UDPTransceiver udp{1024};
	udp.setTarget("192.168.0.80", 2048);

	Point2D point;

	float x = 0;
	while (true) {
		point.set_x(x);
		point.set_y(sin(x));
		x += 0.01f;

		std::string data = point.SerializeAsString();
		udp.sendMsg(MemSlice(data.data(), data.length()));
		udp.getNextMsg();
	}
}

#include <UDPTransceiver.hpp>
#include <Joy.hpp>
#include <constants.hpp>

std::string genCtlMsg(const JoyState& joy)
{
	static bool pwm = true;
	if (joy.buttons.at(BTN_NORTH)) {
		pwm = true;
	}
	if (joy.buttons.at(BTN_SOUTH)) {
		pwm = false;
	}

	float l = joy.axes.at(ABS_BRAKE);
	float r = joy.axes.at(ABS_GAS);
	l *= (joy.buttons.at(BTN_TL) ? (-1.0f) : 1.0f);
	r *= (joy.buttons.at(BTN_TR) ? (-1.0f) : 1.0f);

	if (pwm) {
		r *= 0.8f;
	}

	return fmt::format("{} {} {}\n", pwm ? CMD_PWM : CMD_SPEED, l,r);
}

void tunePID()
{
	UDPTransceiver udp("0.0.0.0:1024", "192.168.0.172:1025");
	// UDPTransceiver udp("0.0.0.0:1024", "0.0.0.0:1025");
	auto x52 = Joy::searchDevice("X52", false);
	Joy ctl(*x52.begin());
	fmt::print("Looks good!\n");
	int msgHeader = 0;
	while (true) {
		const JoyState& joy = ctl.update();
		
		float target = -joy.axes.at(ABS_Z) / 2;
		float p = 20 * joy.axes.at(ABS_THROTTLE);
		float i = 30 * joy.axes.at(ABS_RY);
		float d = 1 * joy.axes.at(ABS_RX);
		std::string cfg = fmt::format("{} {: <+6.3f} {: <+6.3f} {: <+6.3f} {: <+6.3f}\n", CMD_CONFIG_PID, target, p, i, d);
		udp.sendMessage(cfg);
		if (joy.buttons.at(BTN_BASE2)) {
			std::string rst = fmt::format("{}\n", CMD_RESET_PID);
			udp.sendMessage(rst);
		}
		usleep(20 * 1000);
	}
}

int main()
{
	UDPTransceiver udp("0.0.0.0:1024", "192.168.0.172:1025");
	auto nv = Joy::searchDevice("NVIDIA");
	Joy ctl(*nv.begin());
	fmt::print("Looks good!\n");
	while(true) {
		
		auto msgs = udp.getMessages();
		while (!msgs.empty()) {
			fmt::print("{}", msgs.front());
			msgs.pop();
		}
		const JoyState& joy = ctl.update();
		udp.sendMessage(genCtlMsg(joy));

		if (joy.buttons.at(BTN_EAST)) {
			fmt::print("reset\n");
			udp.sendMessage(fmt::format("{}\n", CMD_RESET_PID));
		}

		usleep(50 * 1000);
	}
}
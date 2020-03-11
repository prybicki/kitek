#include <fmt/format.h>
#include <yaml-cpp/yaml.h>

#include <UDPTransceiver.hpp>
#include <PeriodicTask.hpp>
#include <KitekHW.hpp>
#include <Diagnostics.hpp>
#include <TickManager.hpp>

// TODO debug utilities

constexpr auto sides = {LEFT, RIGHT};

bool handleMessage(const KitekMsg& msg, KitekHW& hw)
{
	// fmt::print("{}\n", msg.DebugString());
	if (msg.msg_case() == KitekMsg::kEngCtl) {
		if (msg.engctl().method_case() == EngCtl::kPwm) {
			hw.wheel.at(msg.engctl().side())->setPWM(msg.engctl().pwm());
			// fmt::print("PWM: {} {}\n", msg.engctl().side(), msg.engctl().pwm());
			return true;
		}
		if (msg.engctl().method_case() == EngCtl::kSpeed) {
			hw.wheel.at(msg.engctl().side())->setSpeed(msg.engctl().speed());
			// fmt::print("Speed: {} {}\n", msg.engctl().side(), msg.engctl().speed());
			return true;
		}
		return false;
	}

	if (msg.msg_case() == KitekMsg::kCfgPID) {
		auto p = msg.cfgpid().p();
		auto i = msg.cfgpid().i();
		auto d = msg.cfgpid().d();
		fmt::print("PID: {} {} {}\n", p, i, d);
		for (auto&& side : sides) {
			hw.pid.at(side)->config(p, i, d);
		}
		return true;
	}

	if (msg.msg_case() == KitekMsg::kResetPID) {
		fmt::print("PID: reset\n");
		for (auto&& side : sides) {
			hw.pid.at(side)->reset();
		}
		return true;
	}
	return false;
}

int main(int argc, char** argv)
{
	YAML::Node cfg = YAML::LoadFile("config.yaml");
	YAML::Node stat = YAML::LoadFile("statistics.yaml");
	
	GPIOGuard gpio {
		cfg["gpioDebug"].as<bool>()
	};
	
	TickManagerGPIO tick {
		cfg["loopHz"].as<float>()
	};

	KitekHW hw {cfg};
	fmt::print("sizeof(hw): {} bytes\n", sizeof(hw));

	if (argc > 1) {
		Diagnostics::runTests(argc, argv, hw);
		return 0;
	}

	KitekMsg msg;
	UDPTransceiver udp{1024};
	std::string protocolBuffer;
	std::optional<MemSlice> msgBytes;
	tick.sleepUntilNextTick();
	while (!gpio.mustQuit()) {

		for (auto& tickable : hw.tickables) {
			tickable->tick();
		}

		bool cmdSuccess = true;
		hw.ledB->set(true);
		while ((msgBytes = udp.getNextMsg()).has_value()) {
			msg.ParseFromArray(msgBytes.value().getPtr(), msgBytes.value().getSize());
			if (!handleMessage(msg, hw)) {
				hw.ledR->set(true);
			}
		}
		hw.ledB->set(false);

		for (auto& side : sides) {
			WheelState* wheelState = msg.mutable_wheelstate();
			wheelState->set_side(side);
			wheelState->set_pwm(hw.wheel.at(side)->getPWM());
			wheelState->set_speed(hw.wheel.at(side)->getSpeed());
			wheelState->set_mileage(hw.wheel.at(side)->getMileage());
			auto&& targetSpeed = hw.wheel.at(side)->getTargetSpeed();
			if (targetSpeed.has_value()) {
				wheelState->set_targetspeed(*targetSpeed);
			}
			msg.SerializeToString(&protocolBuffer);
			udp.sendMsg(protocolBuffer);
		}

		hw.ledG->set(true);
		tick.sleepUntilNextTick();
		hw.ledG->set(false);
	}
	
}

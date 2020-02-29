#include <fmt/format.h>
#include <yaml-cpp/yaml.h>

#include <UDPTransceiver.hpp>
#include <PeriodicTask.hpp>
#include <KitekHW.hpp>
#include <diagnostics.hpp>

struct TickManager
{
	TickManager(float hz) :
		periodMs(static_cast<int32_t>(std::lround(1e6f/hz))), 
		tickBeginMs(gpioTick())
	{}

	void sleepUntilNextTick() {
		int32_t elapsed = static_cast<int32_t>(gpioTick() - tickBeginMs);
		int32_t toSleep = periodMs - elapsed;
		if (toSleep > 0) {
			gpioDelay(static_cast<uint32_t>(toSleep));
		}
		tickBeginMs = gpioTick();
	}

private:
	int32_t periodMs;
	uint32_t tickBeginMs;
};

int main(int argc, char** argv)
{
	YAML::Node cfg = YAML::LoadFile("config.yaml");
	YAML::Node stat = YAML::LoadFile("statistics.yaml");
	auto pidP = cfg["pid"]["p"].as<float>();
	auto pidI = cfg["pid"]["i"].as<float>();
	auto pidD = cfg["pid"]["d"].as<float>();
	auto pidMin = cfg["pid"]["min"].as<float>();
	auto pidMax = cfg["pid"]["max"].as<float>();

	GPIOHandle gpio{false};
	L298N engL{22, 27, 17};
	L298N engR{9, 10, 11, true};
	Encoder encL{26, 13};
	Encoder encR{5, 6};
	PID pidL{pidP, pidI, pidD, pidMin, pidMax};
	PID pidR{pidP, pidI, pidD, pidMin, pidMax};
	Wheel wheelL{3840, 0.08, 0.5f, &pidL, &engL, &encL};
	Wheel wheelR{3840, 0.08, 0.5f, &pidR, &engR, &encR};
	Servo servo{8, -90.f, 90.f, 500, 2000};
	// PCF8591 adc{0x48, 1};
	LED_RGB ledRGB{25, 7, 8};

	KitekHW hw = {
		.engL = &engL,
		.engR = &engR,
		.encL = &encL,
		.encR = &encR
	};
	L298N* eng[] = {&engL, &engR};
	Wheel* wheel[] = {&wheelL, &wheelR};
	Tickable* tickables[] = {&wheelL, &wheelR};

	if (argc > 1) {
		runTests(argc, argv, hw);
		return 0;
	}

	UDPTransceiver udp{1024};
	Command command;
	std::optional<MemSlice> msg;
	TickManager tick{cfg["loopHz"].as<float>()};
	std::string protocolBuffer;
	while (!gpio.mustQuit()) {

		for (auto& tickable : tickables) {
			tickable->tick();
		}

		ledRGB.ledB.set(true);
		while ((msg = udp.getNextMsg()).has_value()) {
			command.ParseFromArray(msg.value().getPtr(), msg.value().getSize());
			switch (command.cmd_case()) {
				case Command::CmdCase::kSetPWM:
					eng[command.setpwm().side()]->setPWM(command.setpwm().value());
					break;
				case Command::CmdCase::kSetSpeed:
				default:
					ledRGB.ledR.set(true);
			}
		}
		ledRGB.ledB.set(false);

		Feedback fb;
		for (auto& side : {LEFT, RIGHT}) {
			Feedback_WheelState* wheelState = fb.mutable_wheelstate();
			wheelState->set_side(side);
			wheelState->set_speed(wheel[side]->getSpeed());
			wheelState->set_mileage(wheel[side]->getMileage());
			fb.SerializeToString(&protocolBuffer);
			udp.sendMsg(protocolBuffer);
		}

		ledRGB.ledG.set(true);
		tick.sleepUntilNextTick();
		ledRGB.ledG.set(false);
	}
	
}

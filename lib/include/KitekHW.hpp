#pragma once

#include <PCF8591.hpp>
#include <GPIOGuard.hpp>
#include <L298N.hpp>
#include <Encoder.hpp>
#include <Servo.hpp>
#include <LED.hpp>
#include <PID.hpp>
#include <Wheel.hpp>
#include <Protocol.pb.h>
#include <yaml-cpp/yaml.h>

class KitekHW {
	std::optional<L298N> engL;
	std::optional<L298N> engR;
	std::optional<Encoder> encL;
	std::optional<Encoder> encR;
	std::optional<PID> pidL;
	std::optional<PID> pidR;
	std::optional<Wheel> wheelL;
	std::optional<Wheel> wheelR;
public:
	std::optional<LED_RGB> ledRGB;
	std::optional<LED> ledR;
	std::optional<LED> ledG;
	std::optional<LED> ledB;
	std::optional<Servo> servo;
	std::optional<PCF8591> adc;
	
	std::array<Tickable*, SIDE_COUNT> tickables;
	std::array<Wheel*, SIDE_COUNT> wheel;
	std::array<PID*, SIDE_COUNT> pid;

	KitekHW(const YAML::Node& cfg)
	{
		engL.emplace(
			cfg["engL"]["pinIn1"].as<int>(), 
			cfg["engL"]["pinIn2"].as<int>(),
			cfg["engL"]["pinEnb"].as<int>(),
			cfg["engL"]["rev"].as<bool>()
		);

		engR.emplace(
			cfg["engR"]["pinIn1"].as<int>(), 
			cfg["engR"]["pinIn2"].as<int>(),
			cfg["engR"]["pinEnb"].as<int>(),
			cfg["engL"]["rev"].as<bool>()
		);

		encL.emplace(
			cfg["encL"]["pinA"].as<int>(),
			cfg["encL"]["pinB"].as<int>()
		);

		encR.emplace(
			cfg["encR"]["pinA"].as<int>(),
			cfg["encR"]["pinB"].as<int>()
		);

		auto pidP = cfg["pid"]["p"].as<float>();
		auto pidI = cfg["pid"]["i"].as<float>();
		auto pidD = cfg["pid"]["d"].as<float>();
		auto pidMin = cfg["pid"]["min"].as<float>();
		auto pidMax = cfg["pid"]["max"].as<float>();
		pidL.emplace(pidP, pidI, pidD, pidMin, pidMax);
		pidR.emplace(pidP, pidI, pidD, pidMin, pidMax);
		
		auto ticksPerRev = cfg["wheel"]["ticksPerRev"].as<int>();
		auto diameter = cfg["wheel"]["diameter"].as<float>();
		auto maxSpeed = cfg["wheel"]["maxSpeed"].as<float>();
		wheelR.emplace(ticksPerRev, diameter, maxSpeed, &*pidR, &*engR, &*encR);
		wheelL.emplace(ticksPerRev, diameter, maxSpeed, &*pidL, &*engL, &*encL);
		
		servo.emplace(
			cfg["servo"]["pin"].as<int>(),
			cfg["servo"]["minAngle"].as<float>(),
			cfg["servo"]["maxAngle"].as<float>(),
			cfg["servo"]["minPulse"].as<int>(),
			cfg["servo"]["maxPulse"].as<int>()
		);
		
		ledR.emplace(cfg["led"]["pinR"].as<int>());
		ledG.emplace(cfg["led"]["pinG"].as<int>());
		ledB.emplace(cfg["led"]["pinB"].as<int>());
		ledRGB.emplace(&*ledR, &*ledG, &*ledB);

		tickables = {&*wheelL, &*wheelR};
		wheel = {&*wheelL, &*wheelR};
		pid = {&*pidL, &*pidR};
		
		// PCF8591 adc{0x48, 1};
	}
	friend struct Diagnostics;
};

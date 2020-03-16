#pragma once

#include <L298N.hpp>
#include <Encoder.hpp>
#include <PID.hpp>
#include <Tickable.hpp>
#include <optional>

struct Wheel : Tickable
{
	Wheel(int ticksPerRev, float wheelDiameter, float maxSpeed, PID* pid, L298N* eng, Encoder* enc);

	virtual void tick() override;
	float getMileage();
	float getSpeed();
	float getPWM();
	void setSpeed(float speed);
	void setPWM(float pwm);
	std::optional<float> getTargetSpeed();
	uint32_t getLastUpdateTick();

private:
	EncoderTicks currState;
	EncoderTicks prevState;
	EncoderTicks diff;

	PID* pid;
	L298N* eng;
	Encoder* enc;
	int ticksPerRev;
	float maxSpeed;
	float wheelDiameter;
	std::optional<float> targetSpeedSI;

	float ticksToDist(int ticks);
};

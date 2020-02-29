#pragma once

#include <L298N.hpp>
#include <Encoder.hpp>
#include <PID.hpp>
#include <Tickable.hpp>

struct Wheel : Tickable
{
	Wheel(float ticksPerRev, float wheelDiameter, float maxSpeed, PID* pid, L298N* eng, Encoder* enc);

	virtual void tick() override;
	float getMileage();
	float getSpeed();
	void setSpeed(float speed);

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

	float ticksToDist(int ticks);
};

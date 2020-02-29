#pragma once

#include <cinttypes>

struct PID
{
	float kp;
	float ki;
	float kd;
	float min;
	float max;
	float prevErr = 0.0f;
	float integral = 0.0f;
	uint32_t lastUpdate = 0.0f;

	PID(float p, float i, float d, float min, float max);
	float update(float target, float current);
	void config(float p, float i, float d);
	void reset();
};

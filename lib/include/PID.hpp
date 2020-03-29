#pragma once

#include <cinttypes>

struct PID
{
	float kp;
	float ki;
	float kd;
	float min;
	float max;
	float prevError = 0.0f;
	float prevTarget = 0.0f;
	float integral = 0.0f;

	float error = 0.0f;
	float pOut = 0.0f;
	float iOut = 0.0f;
	float dOut = 0.0f;
	float outRaw = 0.0f;

	PID(float p, float i, float d, float min, float max);
	// last update tick is usually related to current value, hence it's external
	float compute(float target, float current, uint32_t dtUsec);
	void config(float p, float i, float d);
	void reset();
};

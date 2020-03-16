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
	float integral = 0.0f;
	uint32_t lastUpdateTick = 0;

	float error;
	float pOut;
	float iOut;
	float dOut;
	float outRaw;

	PID(float p, float i, float d, float min, float max);
	float compute(float target, float current);
	void config(float p, float i, float d);
	void reset();
};

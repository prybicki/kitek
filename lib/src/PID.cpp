#include <PID.hpp>

#include <pigpio.h>

PID::PID(float p, float i, float d, float min, float max) : min(min), max(max), lastUpdate(gpioTick())
{
    config(p, i, d);
}

float PID::compute(float target, float current) 
{
    uint32_t now = gpioTick();
    float dt = 1e-6 * (static_cast<float>(now) - lastUpdate);
    lastUpdate = now;
    float err = target - current;
    float p = kp * err;
    float i = ki * (integral += err * dt + ((err - prevErr) * dt / 2.0f));
    float d = dt == 0.0f ? 0.0f : kd * ((err - prevErr) / dt);
    float out = p + i + d;
    out = out > max ? max : out;
    out = out < min ? min : out;

    prevErr = err;
    return out;
}

void PID::config(float p, float i, float d) 
{
    kp = p;
    ki = i;
    kd = d;
}

void PID::reset()
{
    integral = 0.0f;
}

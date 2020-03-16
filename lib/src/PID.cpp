#include <PID.hpp>
#include <algorithm>
#include <pigpio.h>

PID::PID(float p, float i, float d, float min, float max) : min(min), max(max), lastUpdateTick(gpioTick())
{
    config(p, i, d);
}

float PID::compute(float target, float current) 
{
    uint32_t now = gpioTick();
    float dt = 1e-6 * (static_cast<float>(now) - lastUpdateTick);
    
    error = target - current;
    pOut = kp * error;
    iOut = ki * (integral += error * dt + ((error - prevError) * dt / 2.0f));
    dOut = dt == 0.0f ? 0.0f : kd * ((error - prevError) / dt);
    outRaw = pOut + iOut + dOut;

    prevError = error;
    lastUpdateTick = now;
    return std::clamp(outRaw, min, max);
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

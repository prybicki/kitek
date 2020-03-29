#include <PID.hpp>
#include <algorithm>
#include <pigpio.h>

PID::PID(float p, float i, float d, float min, float max) :
    min(min),
    max(max)
{
    config(p, i, d);
}

float PID::compute(float target, float current, uint32_t dtUsec) 
{
    float dt = 1e-6f * dtUsec;
    error = target - current;
    
    bool controllable = min <= outRaw && outRaw <= max;
    bool targetChanged = target != prevTarget;
    if (controllable && !targetChanged) {
        integral += (error + prevError) * dt / 2.0f;  // trapeze
    }

    pOut = kp * error;
    iOut = ki * integral;
    dOut = dt == 0.0f ? 0.0f : kd * ((error - prevError) / dt);
    outRaw = pOut + iOut + dOut;


    prevTarget = target;
    prevError = error;
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

#include <Wheel.hpp>
#include <pigpio.h>
#include <cmath>

Wheel::Wheel(float ticksPerRev, float wheelDiameter, float maxSpeed, PID* pid, L298N* eng, Encoder* enc)
        : ticksPerRev(ticksPerRev), wheelDiameter(wheelDiameter), maxSpeed(maxSpeed), pid(pid), eng(eng),  enc(enc)
{ }

void Wheel::tick() {
    prevState = currState;
    currState = enc->getTicks();
    diff = currState - prevState;
}

float Wheel::getMileage() // milleage
{
    float fwd = ticksToDist(currState.forward);
    float bwd = ticksToDist(currState.backward);
    return fwd + bwd;
}

float Wheel::getSpeed() // speed
{
    float fwd = ticksToDist(diff.forward);
    float bwd = ticksToDist(diff.backward);
    double elapsedSecs = 1e-6 * diff.timestap;
    return elapsedSecs > 0 ? (fwd - bwd) / elapsedSecs : 0.0f;
}

float Wheel::ticksToDist(int ticks) {
    return wheelDiameter * M_PI * static_cast<float>(ticks) / ticksPerRev;
}

void Wheel::setSpeed(float speed)
{
    float pwm = pid->update(speed * maxSpeed, getSpeed());
    eng->setPWM(pwm);
}

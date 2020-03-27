#include <Wheel.hpp>
#include <pigpio.h>
#include <cmath>
#include <fmt/format.h>

Wheel::Wheel(int ticksPerRev, float wheelDiameter, float maxSpeed, PID* pid, L298N* eng, Encoder* enc)
        : ticksPerRev(ticksPerRev), wheelDiameter(wheelDiameter), maxSpeed(maxSpeed), pid(pid), eng(eng),  enc(enc)
{ }

void Wheel::tick() {
    prevState = currState;
    currState = enc->getTicks();
    diff = currState - prevState;
    if (targetSpeedSI.has_value()) {
        auto pwm = pid->compute(*targetSpeedSI, getSpeed());
        eng->setPWM(pwm);
    }
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
    double elapsedSecs = 1e-6 * diff.timestamp;
    return elapsedSecs > 0 ? (fwd - bwd) / elapsedSecs : 0.0f;
}

float Wheel::ticksToDist(int ticks) {
    return wheelDiameter * M_PI * static_cast<float>(ticks) / ticksPerRev;
}

void Wheel::setSpeed(float speed)
{
    if (fabs(speed) < 0.01f) {
        speed = 0.0f;
    }
    targetSpeedSI = speed * maxSpeed;

}

void Wheel::setPWM(float pwm)
{
    targetSpeedSI.reset();
    pid->reset();
    eng->setPWM(pwm);
}

float Wheel::getPWM()
{
    return eng->getPWM();
}

std::optional<float> Wheel::getTargetSpeed()
{
    return targetSpeedSI;
}

uint32_t Wheel::getLastUpdateTick()
{
    return getSpeed() == 0 ? gpioTick() : currState.timestamp;
}
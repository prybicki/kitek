#include "Servo.hpp"
#include <pigpio.h>
#include <math.h>
#include <unistd.h>

static float map_range(float a1, float a2, float b1, float b2, float x)
{
	return b1 + (x - a1) * (b2 - b1) / (a2 - a1);
}

static float clamp(float value, float min, float max)
{
	if(value < min) {
		return min;
	}
	if(value > max) {
		return max;
	}
	return value;
}

Servo::Servo(int pin, float minAngle, float maxAngle, int minPulsewidth, int maxPulsewidth)
	: pin(pin),
	  minAngle(minAngle),
	  maxAngle(maxAngle),
	  minPulsewidth(minPulsewidth),
	  maxPulsewidth(maxPulsewidth)
{
	gpioSetMode(pin, PI_OUTPUT);
	setNormalizedAngle(0.0f);
}

void Servo::setPulsewidth(int pulsewidth)
{
	currentPulsewidth = pulsewidth;
	currentAngle =
	map_range(minPulsewidth, maxPulsewidth, minAngle, maxAngle, currentPulsewidth);
	gpioServo(pin, currentPulsewidth);
}

void Servo::setAngle(float angle)
{
	// TODO: why sign is inverted?
	currentAngle = -angle;
	float pulsewidthF  = roundf(
    map_range(minAngle, maxAngle, minPulsewidth, maxPulsewidth, currentAngle));
	currentPulsewidth = clamp(pulsewidthF, minPulsewidth, maxPulsewidth);
	gpioServo((unsigned) pin, (unsigned) currentPulsewidth);
}

float Servo::getAngle()
{
	return currentAngle;
}

void Servo::hello()
{
	setNormalizedAngle(0.0f);
	for (int i = 0; i < 4; i++) {
		setNormalizedAngle(-0.2f);
		usleep(100 * 1000);
		setNormalizedAngle(0.2f);
		usleep(100 * 1000);
	}
	setAngle(0.0f);
}

void Servo::setNormalizedAngle(float normalized_angle)
{
	float angle = map_range(-1, 1, minAngle, maxAngle, normalized_angle);
	setAngle(angle);
}

Servo::~Servo()
{
	setNormalizedAngle(0.0f);
}

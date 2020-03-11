#pragma once

struct Servo
{
	Servo(int pin, float minAngle, float maxAngle, int minPulsewidth, int maxPulsewidth);
	void setPulsewidth(int pulsewidth);
	void setAngle(float angle);
	void setNormalizedAngle(float normalized_angle);
	float getAngle();
	void hello();
	~Servo();

private:
	int pin;
	int minPulsewidth;
	int maxPulsewidth;
	float minAngle;
	float maxAngle;
	int currentPulsewidth;
	float currentAngle;
};

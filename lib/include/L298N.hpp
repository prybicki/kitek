#pragma once

struct L298N
{
	L298N(int pinIn1, int pinIn2, int pinEnable, bool reversed=false);
	void setPWM(float pwm);
	float getPWM();
	~L298N();
	
private:
	int pinIn1;
	int pinIn2;
	int pinEnable;
	float currentPWM;
	bool reversed;
};

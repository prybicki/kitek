#pragma once

struct LED {
	
	LED(int pin);
	void setPWM(float newPWM);
	void set(bool on);
	~LED();

private:
	int pin;
	float pwm;
};

struct LED_RGB
{
	LED_RGB(LED* ledR, LED* ledG, LED* ledB);

	void setRGB(float r, float g, float b);
	void setHSV(float h, float s, float v);
private:
	LED* ledR;
	LED* ledG;
	LED* ledB;
};

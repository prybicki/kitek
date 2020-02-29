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
	LED_RGB(int pinR, int pinG, int pinB);

	void setRGB(float r, float g, float b);
	void setHSV(float h, float s, float v);

	LED ledR;
	LED ledG;
	LED ledB;
};

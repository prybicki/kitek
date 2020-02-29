#include "LED.hpp"

#include <cmath>
#include <pigpio.h>

// Green ~ 100 Ohm (50mA!)
// Yellow, Red ~ 1k Ohm
// Blue ~ 2-4k Ohm

LED::LED(int pin) : pin(pin) 
{
	gpioSetMode(pin, PI_OUTPUT);
	setPWM(0.0f);
}

void LED::setPWM(float newPWM) 
{
	gpioPWM(pin, static_cast<int>(std::roundf(newPWM * gpioGetPWMrange(pin))));
}

void LED::set(bool on)
{
	gpioWrite(pin, on ? PI_HIGH : PI_LOW);
}

LED::~LED()
{
	setPWM(0.0f);
}

LED_RGB::LED_RGB(int pinR, int pinG, int pinB) : ledR(pinR), ledG(pinG), ledB(pinB) { };

void LED_RGB::setRGB(float r, float g, float b)
{
	ledR.setPWM(r);
	ledG.setPWM(g);
	ledB.setPWM(b);
}

void LED_RGB::setHSV(float h, float s, float v)
{
	float c = s * v; // Chroma
	float hPrime = std::fmod(h / 60, 6.f); // H'
	float x = c * (1 - std::fabs(std::fmod(hPrime, 2.f) - 1));
	float m = v - c;

	float r = 0.f;
	float g = 0.f;
	float b = 0.f;

	switch (static_cast<int>(hPrime))
	{
		case 0: r = c; g = x;        break; // [0, 1)
		case 1: r = x; g = c;        break; // [1, 2)
		case 2:        g = c; b = x; break; // [2, 3)
		case 3:        g = x; b = c; break; // [3, 4)
		case 4: r = x;        b = c; break; // [4, 5)
		case 5: r = c;        b = x; break; // [5, 6)
	}

	r += m;
	g += m;
	b += m;

	setRGB(r, g, b);
}
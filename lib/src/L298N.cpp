
#include "L298N.hpp"
#include <pigpio.h>
#include <cmath>

L298N::L298N(int pinIn1, int pinIn2, int pinEnable, bool reversed)
	: pinIn1(pinIn1), 
	  pinIn2(pinIn2), 
	  pinEnable(pinEnable),
	  currentPWM(0.0f),
	  reversed(reversed)
{
	gpioSetMode(pinIn1, PI_OUTPUT);
	gpioSetMode(pinIn2, PI_OUTPUT);
	gpioSetMode(pinEnable, PI_OUTPUT);
	gpioPWM(pinEnable, 0); // initialize PWM on the pin, required to getPWM()
}

void L298N::setPWM(float pwm)
{
	currentPWM = pwm;
	int ipwm = static_cast<int>(std::round(static_cast<float>(gpioGetPWMrange(pinEnable)) * pwm));
	gpioWrite(pinIn1, ipwm >= 0 ? 1 : 0);
	gpioWrite(pinIn2, ipwm >= 0 ? 0 : 1);
	gpioPWM(pinEnable, std::abs(ipwm));
}

float L298N::getPWM()
{
	return currentPWM;
}

L298N::~L298N()
{
	gpioWrite(pinIn1, 0);
	gpioWrite(pinIn2, 0);
	gpioWrite(pinEnable, 0);
}
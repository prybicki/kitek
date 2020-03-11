#pragma once

// Vid-broken RPi has weird MISO (BCM19), floating on high
struct GPIOGuard 
{
	GPIOGuard(bool debug=false);
	bool mustQuit();
	~GPIOGuard();
};

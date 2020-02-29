#pragma once

// Vid-broken RPi has weird MISO (BCM19), floating on high
struct GPIOHandle 
{
	GPIOHandle(bool debug=false);
	bool mustQuit();
	~GPIOHandle();
};

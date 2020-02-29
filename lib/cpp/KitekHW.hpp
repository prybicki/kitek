#pragma once

#include <PCF8591.hpp>
#include <GPIOHandle.hpp>
#include <L298N.hpp>
#include <Encoder.hpp>
#include <Servo.hpp>
#include <LED.hpp>
#include <PID.hpp>
#include <Wheel.hpp>
#include <Protocol.pb.h>

struct KitekHW {
	GPIOHandle* gpio;
	PID* pidL;
	PID* pidR;
	L298N* engL;
	L298N* engR;
	Encoder* encL;
	Encoder* encR;
	Wheel* wheelL;
	Wheel* wheelR;
	PCF8591* adc;
	Servo* servo;
	LED_RGB* ledRGB;
};


#include <fmt/format.h>
#include <pigpio.h>
#include <L298N.hpp>
#include <KitekHW.hpp>
#include <Encoder.hpp>
#include <string>
#include <Diagnostics.hpp>

void Diagnostics::testEngines(KitekHW& hw)
{
	const int ENGINE_TEST_SEC = 2;
	const float ENGINE_TEST_PWM = 0.5f;

    fmt::print("Left forward\n");
    hw.engL->setPWM(ENGINE_TEST_PWM);
    gpioSleep(PI_TIME_RELATIVE, ENGINE_TEST_SEC, 0);

    fmt::print("Left backward\n");
    hw.engL->setPWM(-ENGINE_TEST_PWM);
    gpioSleep(PI_TIME_RELATIVE, ENGINE_TEST_SEC, 0);

    hw.engL->setPWM(0.0);

    fmt::print("Right forward\n");
    hw.engR->setPWM(ENGINE_TEST_PWM);
    gpioSleep(PI_TIME_RELATIVE, ENGINE_TEST_SEC, 0);

    fmt::print("Right backward\n");
    hw.engR->setPWM(-ENGINE_TEST_PWM);
    gpioSleep(PI_TIME_RELATIVE, ENGINE_TEST_SEC, 0);

    hw.engR->setPWM(0);
}

void Diagnostics::printEncoders(KitekHW& hw)
{
    EncoderTicks ticksL = hw.encL->getTicks();
    EncoderTicks ticksR = hw.encR->getTicks();
    fmt::print("Encoder L: f{} b{} e{}\n", ticksL.forward, ticksL.backward, ticksL.empty);
    fmt::print("Encoder R: f{} b{} e{}\n", ticksR.forward, ticksR.backward, ticksR.empty);
}

void Diagnostics::testEncoders(KitekHW& hw)
{
    GPIOGuard gpio;
    while(!gpio.mustQuit()) {
        printEncoders(hw);
        gpioSleep(PI_TIME_RELATIVE, 0, 100 * 1000);
    }
}

void Diagnostics::runTests(int argc, char** argv, KitekHW& hw)
{
	for (int i = 1; i < argc; ++i) {
		if (std::string(argv[i]) == "eng") {
			testEngines(hw);
		}
		
		if (std::string(argv[i]) == "enc") {
			testEncoders(hw);
		}
		
	}
}

// void testHardware(Kitek& hw)
// {
//     fmt::print("Servo\n");
//     hw.servo->hello();

//     hw.adc->update();
//     fmt::print("ADC: CH0={} CH1={} CH2={} CH3={}\n",
//                hw.adc->analogRead(0),
//                hw.adc->analogRead(1),
//                hw.adc->analogRead(2),
//                hw.adc->analogRead(3));
// }


// #include <unordered_map>
// void testPins(GPIOGuard& gpio, std::initializer_list<int> pins = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27})
// {
//     std::unordered_map<int, int> pinToMode;
//     for(const auto& pin : pins) {
//         pinToMode[pin] = gpioGetMode(pin);
//         fmt::print("pin {} mode = {}\n", pin, pinToMode[pin]);
//         int code = gpioWrite(pin, 1);
//         if (code != 0) {
//             fmt::print("\tfailed to set pin {} to high\n", pin);
//         }
//     }
//     fmt::print("all pins set to high, verify and press ctrl+c\n");

//     while(!gpio.mustQuit())
//         ;

//     for(const auto& pin : pins) {
//         int code = gpioWrite(pin, 0);
//         if (code != 0) {
//             fmt::print(stderr, "failed to set pin {} to low\n", pin);
//         }
//     }
//     fmt::print("all pins set to low, verify and press ctrl+c\n");

//     while(!gpio.mustQuit())
//         ;

//     for (const auto& pin : pins) {
//         int code = gpioSetMode(pin, pinToMode[pin]);
//         if (code != 0) {
//             fmt::print("\nfailed to set original mode {} on pin {}\n", pinToMode[pin], pin);
//         }
//     }
// }

// #include <chrono>
// void testLatency()
// {
// 	// chrono: 851.759 ns
// 	// gpioTick: 133.43 ns
// 	const int TRIALS = 100000;
// 	{
// 		std::vector<double> intervals(TRIALS);
// 		auto then = std::chrono::high_resolution_clock::now();
// 		for (auto& interval : intervals) {
// 			auto now = std::chrono::high_resolution_clock::now();
// 			interval = std::chrono::duration_cast<std::chrono::nanoseconds>(now - then).count();
// 			then = now;
// 		}
// 		double avgInterval = std::accumulate(intervals.begin(), intervals.end(), 0.0) / intervals.size();
// 		fmt::print("chrono: {} ns\n", avgInterval);
// 	}

// 	gpioInitialise();
// 	{
// 		std::vector<double> intervals(TRIALS);
// 		auto then = static_cast<int>(gpioTick());
// 		for (auto& interval : intervals) {
// 			auto now = static_cast<int>(gpioTick());
// 			interval = (now - then) * 1000;
// 			then = now;
// 		}
// 		double avgInterval = std::accumulate(intervals.begin(), intervals.end(), 0.0) / intervals.size();
// 		fmt::print("gpioTick: {} ns\n", avgInterval);
// 	}
// }

// void tunePID(Kitek& hw)
// {
// 	PID pid(0.0f, 0.0f, 0.0f, -1.0f, 1.0f);
// 	UDPTransceiver udp("0.0.0.0:1025", "192.168.0.80:1024");
// 	float s = 0.0f, p = 0.0f, i = 0.0f, d = 0.0f;
// 	while(!hw.gpio->mustQuit()) {
// 		auto msgs = udp.getMessages();
// 		while (!msgs.empty()) {
// 			auto&& msg = msgs.front();
// 			int msgCode = 0;
// 			sscanf(msg.c_str(), "%d", &msgCode);
// 			if (msgCode == CMD_CONFIG_PID) {
// 				sscanf(msg.c_str(), "%*d %f %f %f %f", &s, &p, &i, &d);
// 			}
// 			if (msgCode == CMD_RESET_PID) {
// 				pid.reset();
// 			}
// 			msgs.pop();
// 		}
// 		pid.config(p, i, d);
// 		hw.wheelL->update();
// 		float curr = hw.wheelL->getSpeed();
// 		float next = pid.update(s, curr);
// 		hw.engL->setPWM(next);
// 		gpioSleep(PI_TIME_RELATIVE, 0, 20 * 1000);
// 	}
// }


// void testEncoders(GPIOGuard& gpio, Encoder& l, Encoder& r);
// void testADC(GPIOGuard& gpio, PCF8591& adc);
// void testHardware(Kitek& hw);

// #include <pigpio.h>
// void testADC(GPIOGuard& gpio, PCF8591& adc)
// {
//     while(!gpio.mustQuit()) {
//         adc.update();
//         fmt::print("ADC: CH0={} CH1={} CH2={} CH3={}\n",
//                    adc.analogRead(0),
//                    adc.analogRead(1),
//                    adc.analogRead(2),
//                    adc.analogRead(3));
//         gpioSleep(PI_TIME_RELATIVE, 0, 100 * 1000);
//     }
// }


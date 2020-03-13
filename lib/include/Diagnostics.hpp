#pragma once

#include <KitekHW.hpp>

struct Diagnostics {
	static void runTests(int argc, char** argv, KitekHW& hw);
	static void testEncoders(KitekHW& hw);
	static void printEncoders(KitekHW& hw);
	static void testEngines(KitekHW& hw);
	static void printClocksLatency();
	
	static double testChronoLatencyNs(size_t trials);
	static double testGpioTickLatencyNs(size_t trials);
	static double testClockAmplifierGpioTickLatencyNs(size_t trials);
};


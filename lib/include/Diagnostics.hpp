#pragma once

#include <KitekHW.hpp>

struct Diagnostics {
	static void runTests(int argc, char** argv, KitekHW& hw);
	static void testEncoders(KitekHW& hw);
	static void printEncoders(KitekHW& hw);
	static void testEngines(KitekHW& hw);
};


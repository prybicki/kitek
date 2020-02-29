#include "GPIOHandle.hpp"
#include <pigpio.h>
#include <signal.h>
#include <stdexcept>

static GPIOHandle* instance = nullptr;

static volatile bool signalOccured = false;
static void signalHandler(int num)
{
	signalOccured = true;
}

GPIOHandle::GPIOHandle(bool debug)
{
    if (instance != nullptr) {
        throw std::runtime_error("double init detected");
    }

    if (debug) {
        gpioCfgInternals(984762879, 4);
    }

	int v = gpioInitialise();
	if (v == PI_INIT_FAILED) {
		throw std::runtime_error("pigpio init failed");
	}

	struct sigaction act = {0};
	act.sa_handler = signalHandler;
	sigaction(SIGINT, &act, NULL);

	instance = this;
}

bool GPIOHandle::mustQuit()
{
	if (signalOccured) {
		signalOccured = false;
		return true;
	}
	return false;
}

GPIOHandle::~GPIOHandle() 
{
	gpioTerminate();
    instance = nullptr;
}

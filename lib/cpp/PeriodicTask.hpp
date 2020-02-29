#pragma once

#include <functional>
#include <pigpio.h>

template<typename F>
struct PeriodicTask
{
	PeriodicTask(std::function<F> task, uint32_t periodUsecs) :
		task(task), 
		periodUsecs(periodUsecs), 
		lastExecution(gpioTick()) 
	{ }
	void tick() {
		uint32_t now = gpioTick();
		if (now - lastExecution >= periodUsecs) {
			lastExecution = now;
			task.invoke();
		}
	}

private:
	std::function<F> task;
	uint32_t periodUsecs;
	uint32_t lastExecution;
};

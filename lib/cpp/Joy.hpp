#pragma once

#include <unordered_map>
#include <queue>
#include <set>
#include <filesystem>
#include <libevdev-1.0/libevdev/libevdev.h>

struct JoyState
{
	std::unordered_map<int, bool> buttons;
	std::unordered_map<int, float> axes;
};

namespace fs = std::filesystem;

struct Joy
{
	Joy(fs::path devPath);
	~Joy();

	const JoyState& update();
	const char* btnName(int btn);
	const char* axisName(int btn);

	static std::set<fs::path> searchDevice(const std::string& nameSubstring, bool verbose=true);

private:
    int fd = 0;
    struct libevdev *dev = NULL;
	std::unordered_map<int, int> axisZero;
	std::unordered_map<int, const input_absinfo*> axisInfo;

	// Buttons that received press and release during previous update loop
	// They are set to 1 in public state, but reset in next update.
	std::queue<int> stickyButtons;
	JoyState state;

	float axisValueToFloat(int axis, int value);
};

#pragma once

#include <unordered_map>
#include <set>
#include <filesystem>
#include <libevdev-1.0/libevdev/libevdev.h>
#include <Tickable.hpp>

struct JoyState
{
	std::unordered_map<int, bool> button;
	std::unordered_map<int, float> axis;
	std::unordered_map<int, bool> buttonChanged;
	std::unordered_map<int, bool> axisChanged;
};

namespace fs = std::filesystem;

struct Joy : Tickable
{
	Joy(fs::path devPath);
	~Joy();

	virtual void tick() final;
	const JoyState& getState() const;

	const char* getButtonName(int btn) const;
	const char* getAxisName(int btn) const;

	static std::set<fs::path> searchDevice(const std::string& nameSubstring, bool verbose=true);

private:
	int fd = 0;
	struct libevdev *dev = NULL;
	
	JoyState state;
	std::unordered_map<int, int> axisZero;
	std::unordered_map<int, const input_absinfo*> axisInfo;

	// Buttons that received press and release during previous update loop
	// They are set to 1 in public state, but reset in next update.
	std::set<int> stickyButtons;

	float axisValueToFloat(int axis, int value);
};

#include <fmt/format.h>
template <>
struct fmt::formatter<Joy> {
	constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

	template <typename FormatContext>
	auto format(const Joy& joy, FormatContext& ctx) {
		auto& state = joy.getState();
		format_to(ctx.out(), "buttons: {{\n");
		for (auto& btn : state.button) {
			format_to(ctx.out(), "\t{} {},\n", joy.getButtonName(btn.first), btn.second);
		}
		format_to(ctx.out(), "}}\n");

		format_to(ctx.out(), "axes: {{");
		for (auto& axis : state.axis) {
			format_to(ctx.out(), "\t{} {},\n", joy.getAxisName(axis.first), axis.second);
		}
		return format_to(ctx.out(), "}}");
		
	}
};

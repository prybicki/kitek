#include <UDPTransceiver.hpp>
#include <Joy.hpp>
#include <Protocol.pb.h>
#include <fmt/format.h>
#include <Tickable.hpp>
#include <TickManager.hpp>

constexpr int PID_VALUE_COUNT = 3;
constexpr auto sides = {LEFT, RIGHT};

template<typename T, T min, T max>
struct BoundedInteger
{
	T value;

	BoundedInteger<T, min, max>& operator+=(T rhs) {
		value = std::clamp(value+rhs, min, max);
		return *this;
	}

	operator T() {
		return value;
	}
};

template<typename I, typename F>
I getSign(F v) {
	if (v > static_cast<F>(0)) {
		return 1;
	}
	if (v < static_cast<F>(0)) {
		return -1;
	}
	return 0;
}

template<typename T, T count>
using CyclicIndex = BoundedInteger<T, static_cast<T>(0), count-1>;

struct NVController : Tickable {
	NVController() {
		auto nv = Joy::searchDevice("NVIDIA");
		if (nv.empty()) {
			return;
		}
		pad.emplace(*nv.begin());
	}

	virtual void tick() override
	{
		if (!pad.has_value()) {
			return;
		}
		pad->tick();
		const JoyState& state = pad->getState();
		
		if (state.button.at(BTN_NORTH)) {
			engCtlMethod = EngCtl::kPwm;
		}
		if (state.button.at(BTN_SOUTH)) {
			engCtlMethod = EngCtl::kSpeed;
		}
		if (state.buttonChanged.at(BTN_EAST) && state.button.at(BTN_EAST)) {
			pidPendingReset = true;
		}

		drive.at(LEFT) = state.axis.at(ABS_BRAKE) * (state.button.at(BTN_TL) ? (-1.0f) : 1.0f);
		drive.at(RIGHT) = state.axis.at(ABS_GAS) * (state.button.at(BTN_TR) ? (-1.0f) : 1.0f);
		
		bool hatXChanged = state.axisChanged.at(ABS_HAT0X);
		bool hatYChanged = state.axisChanged.at(ABS_HAT0Y);
		int hatXValue = getSign<int, float>(state.axis.at(ABS_HAT0X));
		int hatYValue = (-1) * getSign<int, float>(state.axis.at(ABS_HAT0Y));
		if (hatXChanged) {
			pidValueIdx += hatXValue;
		}
		if (hatYChanged) {
			pidValue.at(pidValueIdx) += hatYValue;
			pidCfgDirty = true;
		}
	}

	std::vector<KitekMsg> getMessages()
	{
		if(!pad.has_value()) {
			return {};
		}
		KitekMsg msg;
		std::vector<KitekMsg> messages;
		
		// Driving
		{
			for (auto&& side : sides) {
				EngCtl* engCtl = msg.mutable_engctl();
				engCtl->set_side(side);
				if (engCtlMethod == EngCtl::kPwm) {
					engCtl->set_pwm(drive.at(side));
				}
				if (engCtlMethod == EngCtl::kSpeed) {
					engCtl->set_speed(drive.at(side));
				}
				messages.push_back(msg);
			}
		}

		if (pidCfgDirty) {
			PIDConfig* pidCfgPtr = msg.mutable_cfgpid();
			pidCfgPtr->set_p(pidValue.at(0));
			pidCfgPtr->set_i(pidValue.at(1));
			pidCfgPtr->set_d(pidValue.at(2));
			messages.push_back(msg);
			pidCfgDirty = false;
		}

		if (pidPendingReset) {
			msg.mutable_resetpid();
			messages.push_back(msg);
			pidPendingReset = false;
		}
		return messages;
	}

	bool isConnected()
	{
		return pad.has_value();
	}

	std::optional<Joy> pad;

	std::array<float, SIDE_COUNT> drive;
	EngCtl::MethodCase engCtlMethod {EngCtl::kPwm};

	bool pidCfgDirty {false};
	bool pidPendingReset {false};
	std::array<float, PID_VALUE_COUNT> pidValue {16, 24, 0};
	CyclicIndex<int, PID_VALUE_COUNT> pidValueIdx {0};
};

int main(int argc, char** argv)
{
	TickManagerChrono tick {50};
	NVController ctl;
	UDPTransceiver udp {2048};
	UDPTransceiver gui {2049};
	udp.setTarget("192.168.0.171", 1024);
	gui.setTarget("127.0.0.1", 4096);
	fmt::print("NVIDIA Controller {}\n", ctl.isConnected() ? "connected" : "disconnected");

	KitekMsg msg;
	std::optional<MemSlice> msgBytes;
	std::string protocolBuffer;

	// Send initial message to let the driver know base address.
	msg.mutable_resetpid();
	msg.SerializeToString(&protocolBuffer);
	udp.sendMsg(protocolBuffer);

	tick.sleepUntilNextTick();
	while(true) {
		while ((msgBytes = udp.getNextMsg()).has_value()) {
			msg.ParseFromArray(msgBytes->getPtr(), msgBytes->getSize());
			if (msg.msg_case() == KitekMsg::MsgCase::kWheelState) {
				gui.sendMsg(*msgBytes);
			}
			if (msg.msg_case() == KitekMsg::MsgCase::kPidState) {
				gui.sendMsg(*msgBytes);
			}
		}
		
		ctl.tick();

		for (auto& msg : ctl.getMessages()) {
			msg.SerializeToString(&protocolBuffer);
		}

		tick.sleepUntilNextTick();
	}
}
#pragma once

#include <array>
#include "../config.h"

enum class BridgeState
{
	OFF,
	FORWARD,
	REVERSE
};

class ReactorState
{
public:
	struct Devices
	{
		std::array<BridgeState, config::HBridge::pins.size()> hbridge;
		std::array<bool, config::fet.size()> fet;
		bool led;
		bool motor;
	};

	ReactorState();

	bool initialize();

	bool enable();
	bool is_enabled();
	bool shutdown();

	bool changeHBridge(size_t num, BridgeState state);
	bool changeFET(size_t num, bool is_enabled);
	bool changeLED(bool is_enabled);
	bool changeMotor(bool is_enabled);

	//motor
	const Devices& read() const;

private:
	bool powerHBridge(bool is_enabled);

	Devices devices_state;
	bool enabled;
};

const char* bridgeStateConvert(BridgeState state);
BridgeState bridgeStateConvert(const String& state);

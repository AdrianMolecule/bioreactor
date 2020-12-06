#pragma once

#include <array>
#include "../config.h"

enum class BridgeState
{
	OFF,
	FORWARD,
	REVERSE
};

class Actuators
{
public:
	struct Devices
	{
		std::array<BridgeState, config::HBridge::pins.size()> hbridge;
		std::array<bool, config::fet.size()> fet;
		bool led;
		bool motor;
	};

	Actuators();

	bool initialize();

	bool shutdown();

	bool changeHBridge(size_t num, BridgeState state);
	bool changeFET(size_t num, bool is_enabled);
	bool changeLED(bool is_enabled);
	bool changeMotor(bool is_enabled);

	//motor
	const Devices& read() const;

private:
	bool powerHBridge(bool is_enabled);

	Devices _devices_state;
};

const char* bridgeStateConvert(BridgeState state);
BridgeState bridgeStateConvert(const String& state);
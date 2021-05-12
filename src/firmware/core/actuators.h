#pragma once

#include <array>
#include <ESP_FlexyStepper.h>
#include "../config.h"
#include "pwm_device.h"

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
		std::array<PWMDevice, config::fet.size()> fet;
		bool led;
		bool motor;
	};

	Actuators();

	bool initialize();

	bool shutdown();

	bool changeHBridge(size_t num, BridgeState state);
	bool changeFET(size_t num, unsigned short power);
	bool changeLED(bool is_enabled);
	bool changeMotor(bool is_enabled);
	void runMotor();

	//motor
	const Devices& read() const;

private:
	bool powerHBridge(bool is_enabled);

	Devices _devices_state;
	ESP_FlexyStepper _stepper;
};

const char* bridgeStateConvert(BridgeState state);
BridgeState bridgeStateConvert(const String& state);

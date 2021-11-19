#pragma once

#include <array>
#include <ESP_FlexyStepper.h>
#include <ArduinoJson.h>
#include "../config.h"
#include "pwm_device.h"

enum class BridgeState
{
	FORWARD,
	REVERSE
};

class Actuators
{
public:
	struct BridgeDevice
	{
		BridgeState state;
		PWMDevice pwm_ctrl;
	};

	struct Devices
	{
		std::array<BridgeDevice, config::HBridge::pins.size()> hbridge;
		std::array<PWMDevice, config::fet.size()> fet;
		bool led;
		bool motor;
	};

	Actuators();

	bool initialize();

	bool shutdown();

	bool changeHBridge(size_t num, BridgeState state, uint8_t power);
	bool changeFET(size_t num, uint8_t power);
	bool changeLED(bool is_enabled);
	bool changeMotor(bool is_enabled);
	void runMotor();

	void serializeState(JsonObject&) const;

private:
	bool powerHBridge(bool is_enabled);

	Devices _devices_state;
	ESP_FlexyStepper _stepper;
};

const char* bridgeStateConvert(BridgeState state);
BridgeState bridgeStateConvert(const String&& state);

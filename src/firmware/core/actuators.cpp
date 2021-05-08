#include <Arduino.h>
#include "core/actuators.h"

Actuators::Actuators()
{
	initialize();
	shutdown();
}

bool Actuators::initialize()
{
	pinMode(config::motor::step,OUTPUT);
	pinMode(config::motor::direction,OUTPUT);

	_stepper.connectToPins(config::motor::step, config::motor::direction);
	_stepper.setSpeedInStepsPerSecond(1000);
	_stepper.setAccelerationInStepsPerSecondPerSecond(1000);

	for(auto& fet_pin : config::fet)
	{
		pinMode(fet_pin, OUTPUT);
	}

	for(const auto& input : config::HBridge::pins)
	{
		pinMode(input.A, OUTPUT);
		pinMode(input.B, OUTPUT);
	}

	pinMode(config::HBridge::power, OUTPUT);
	pinMode(config::led_pin, OUTPUT);
	return true;
}

bool Actuators::shutdown()
{
	bool result = true;

	for(size_t i = 0; i < config::fet.size(); ++i)
		result &= changeFET(i, false);

	for(size_t i = 0; i < config::HBridge::pins.size(); ++i)
	{
		result &= changeHBridge(i, BridgeState::OFF);
	}

	result &= powerHBridge(false);
	result &= changeLED(false);
	result &= changeMotor(false);
	return result;
}

bool Actuators::powerHBridge(bool is_enabled)
{
	digitalWrite(config::HBridge::power, is_enabled);
	return true;
}

bool Actuators::changeHBridge(size_t num, BridgeState state)
{
	if(num >= config::HBridge::pins.size())
		return false;

	bool a_key = false;
	bool b_key = false;

	if( state != BridgeState::OFF )
	{
		if( state == BridgeState::FORWARD )
		{
			a_key = true;
			b_key = false;
		}
		else
		{
			a_key = false;
			b_key = true;
		}

		powerHBridge(true);
	}

	digitalWrite(config::HBridge::pins[num].A, a_key);
	digitalWrite(config::HBridge::pins[num].B, b_key);
	_devices_state.hbridge[num] = state;

	return true;
}

bool Actuators::changeFET(size_t num, bool is_enabled)
{
	if(num >= config::fet.size())
		return false;

	//digitalWrite(config::fet[num], !is_enabled);	// reverse bool because FETs enabled by LOW signal
	_devices_state.fet[num].setPower(is_enabled ? 100 : 0);
	return true;
}

bool Actuators::changeLED(bool is_enabled)
{
	digitalWrite(config::led_pin, is_enabled);
	_devices_state.led = is_enabled;
	return true;
}

bool Actuators::changeMotor(bool is_enabled)
{
	digitalWrite(config::motor::power, is_enabled);

	if(is_enabled && !_stepper.isStartedAsService())
		_stepper.startAsService();
	else if(!is_enabled && _stepper.isStartedAsService())
		_stepper.stopService();


	_devices_state.motor = is_enabled;
	return true;
}

void Actuators::runMotor()
{
	_stepper.setTargetPositionRelativeInRevolutions(10000);
}

const Actuators::Devices& Actuators::read() const
{
	return _devices_state;
}

const char* bridgeStateConvert(BridgeState state)
{
	if( state == BridgeState::FORWARD )
		return "Forward";
	else if( state == BridgeState::REVERSE )
		return "Reverse";

	return "Off";
}

BridgeState bridgeStateConvert(const String& state)
{
	if( state == "Forward" )
		return BridgeState::FORWARD;
	else if( state == "Reverse" )
		return BridgeState::REVERSE;

	return BridgeState::OFF;
}

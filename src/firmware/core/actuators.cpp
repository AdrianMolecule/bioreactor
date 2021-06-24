#include <Arduino.h>
#include "core/actuators.h"

Actuators::Actuators()
{
	initialize();
	shutdown();
	powerHBridge(true);
}

bool Actuators::initialize()
{
	pinMode(config::motor::step,OUTPUT);
	pinMode(config::motor::direction,OUTPUT);

	_stepper.connectToPins(config::motor::step, config::motor::direction);
	_stepper.setSpeedInStepsPerSecond(1000);
	_stepper.setAccelerationInStepsPerSecondPerSecond(1000);

	for(size_t i = 0; i < config::fet.size(); ++i)
	{
		_devices_state.fet[i] = PWMDevice(config::fet[i]);
	}

	for(size_t i = 0; i < config::HBridge::pins.size(); ++i)
	{
		_devices_state.hbridge[i].pwm_ctrl = PWMDevice(config::HBridge::pins[i].A);
		pinMode(config::HBridge::pins[i].B, OUTPUT);
		digitalWrite(config::HBridge::pins[i].B, LOW);
	}

	pinMode(config::HBridge::power, OUTPUT);

	pinMode(config::led_pin, OUTPUT);
	return true;
}

bool Actuators::shutdown()
{
	bool result = true;

	for(size_t i = 0; i < config::fet.size(); ++i)
		result &= changeFET(i, 0);

	for(size_t i = 0; i < config::HBridge::pins.size(); ++i)
	{
		result &= changeHBridge(i, BridgeState::FORWARD, 0);
	}

	result &= changeLED(false);
	result &= changeMotor(false);
	return result;
}

bool Actuators::powerHBridge(bool is_enabled)
{
	digitalWrite(config::HBridge::power, is_enabled);
	return true;
}

bool Actuators::changeHBridge(size_t num, BridgeState state, uint8_t power)
{
	if(num >= config::HBridge::pins.size())
		return false;

	if(_devices_state.hbridge[num].state == state && power == _devices_state.hbridge[num].pwm_ctrl.getPower())
		return true;

	if(_devices_state.hbridge[num].state != state)
	{
		if( state == BridgeState::FORWARD )
		{
			_devices_state.hbridge[num].pwm_ctrl.reattachPin( config::HBridge::pins[num].A );
		}
		else
		{
			_devices_state.hbridge[num].pwm_ctrl.reattachPin( config::HBridge::pins[num].B );
		}
	}

	Serial.printf("Actuators::changeHBridge: set %d old_power %d power %d\n",num, _devices_state.hbridge[num].pwm_ctrl.getPower(), power);
	_devices_state.hbridge[num].state = state;
	_devices_state.hbridge[num].pwm_ctrl.setPower(power);

	return true;
}

bool Actuators::changeFET(size_t num, uint8_t power)
{
	if(num >= config::fet.size())
		return false;

	const uint8_t actual_power = PWMDevice::MAX_POWER - power;

	if(actual_power == _devices_state.fet[num].getPower())
		return true;

	Serial.printf("Actuators::changeFET: set %d power %d\n",num, power);
	_devices_state.fet[num].setPower(actual_power);		// reverse power because FET is enabled by LOW signal
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

void Actuators::serializeState(JsonObject& state) const
{
	for(size_t i = 0; i < config::fet.size(); ++i)
		state["fet"][i] = PWMDevice::MAX_POWER - _devices_state.fet[i].getPower();	// reverse power because FET is enabled by LOW signal

	for(size_t i = 0; i < config::HBridge::pins.size(); ++i)
	{
		state["hbridge"][i]["state"] = bridgeStateConvert(_devices_state.hbridge[i].state);
		state["hbridge"][i]["power"] = _devices_state.hbridge[i].pwm_ctrl.getPower();
	}

	state["led"] = _devices_state.led;
	state["motor"] = _devices_state.motor;
}

const char* bridgeStateConvert(BridgeState state)
{
	if( state == BridgeState::FORWARD )
		return "Forward";

	return "Reverse";
}

BridgeState bridgeStateConvert(const String&& state)
{
	if( state == "Forward" )
		return BridgeState::FORWARD;

	return BridgeState::REVERSE;
}


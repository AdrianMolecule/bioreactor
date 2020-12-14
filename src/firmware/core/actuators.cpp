#include <Arduino.h>
#include <core/actuators.h>

Actuators::Actuators()
{
	initialize();
	shutdown();
}

bool Actuators::initialize()
{
	pinMode(config::motor::step,OUTPUT);
	pinMode(config::motor::direction,OUTPUT);

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

	digitalWrite(config::fet[num], !is_enabled);	// reverse bool because FETs enabled by LOW signal
	_devices_state.fet[num] = is_enabled;
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
	_devices_state.motor = is_enabled;
	return true;
}

void Actuators::runMotor()
{
	digitalWrite(config::motor::direction,HIGH); //Enables the motor to move in a perticular direction
	// for one full rotation required 200 pulses
	for(int x = 0; x < 900; x++){
	  digitalWrite(config::motor::step,HIGH);
	  delayMicroseconds(500);
	  digitalWrite(config::motor::step,LOW);
	  delayMicroseconds(500);
	}
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

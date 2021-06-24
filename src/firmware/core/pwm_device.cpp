#include "pwm_device.h"

uint8_t PWMDevice::lastChannel = 0;

PWMDevice::PWMDevice(uint8_t pin1) : _power{0}, _pin{pin1}
{
	//30% is about the stall power, rocker at 80% we get about 40RPM
	if (lastChannel <= 15) {
		_channel = lastChannel;
		lastChannel++;
	} else {
		lastChannel = -1;
		Serial.print("No More Channels");
	}
	pinMode(_pin, OUTPUT);

	// configure PWM functionality
	ledcSetup(_channel, FREQUENCY_OF_CHANGING_PWM, RESOLUTION);
	// attach the channel to the GPIO2 to be controlled
	Serial.printf("PWMDevice::PWMDevice: attach %d to channel %d\n", _pin, _channel);
	ledcAttachPin(_pin, _channel);
}

void PWMDevice::setPower(uint8_t power)
{
	_power = std::min<uint8_t>(power, MAX_POWER);
	unsigned short calculatedPower = _power * 10.23;	// range is 0 ->1023  where 0 is max power;
	ledcWrite(_channel, calculatedPower);
}

uint8_t PWMDevice::getPower() const
{
	return _power;
}

uint8_t PWMDevice::getChannel() {
	return _channel;
}

void PWMDevice::reattachPin(uint8_t pin1)
{
	ledcDetachPin(_pin);
	pinMode(_pin, OUTPUT);
	digitalWrite(_pin, LOW);

	_pin = pin1;
	pinMode(_pin, OUTPUT);
	ledcAttachPin(_pin, _channel);

}

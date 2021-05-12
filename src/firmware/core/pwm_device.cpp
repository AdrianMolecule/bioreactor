#include "pwm_device.h"

uint8_t PWMDevice::lastChannel = 0;

PWMDevice::PWMDevice(unsigned char pin1) : power_{0}
{
	//30% is about the stall power, rocker at 80% we get about 40RPM
	if (lastChannel <= 15) {
		channel_ = lastChannel;
		lastChannel++;
	} else {
		lastChannel = -1;
		Serial.print("No More Channels");
	}
	pinMode(pin1, OUTPUT);

	// configure PWM functionality
	ledcSetup(channel_, FREQUENCY_OF_CHANGING_PWM, RESOLUTION);
	// attach the channel to the GPIO2 to be controlled
	ledcAttachPin(pin1, channel_);
}

void PWMDevice::setPower(unsigned short power)
{
	power_ = std::min<unsigned short>(power, 100);
	unsigned short calculatedPower = power_ * 10.23;	// range is 0 ->1023  where 0 is max power;
	ledcWrite(channel_, calculatedPower);
}

uint8_t PWMDevice::getPower() const
{
	return power_;
}

uint8_t PWMDevice::getChannel() {
	return channel_;
}


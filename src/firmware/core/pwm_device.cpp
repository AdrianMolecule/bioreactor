#include "pwm_device.h"

void PWMDevice::setup(unsigned char pin1, unsigned char pin2)
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
	pinMode(pin2, OUTPUT);

	// configure PWM functionality
	ledcSetup(channel_, FREQUENCY_OF_CHANGING_PWM, RESOLUTION);
	// attach the channel to the GPIO2 to be controlled
	ledcAttachPin(pin1, channel_);
}

PWMDevice::~PWMDevice() {
	// TODO Auto-generated destructor stub
}

void PWMDevice::setPower(uint8_t power)
{
	power_ = power;
	uint8_t calculatedPower = power_ * 10.23;	// range is 0 ->1023  where 0 is max power;
	ledcWrite(channel_, calculatedPower);
}

uint8_t PWMDevice::getChannel() {
	return channel_;
}


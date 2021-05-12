#pragma once

#include <Arduino.h>

const int RESOLUTION = 10; //Resolution 8 does not seem to work, 10, 12, 15
const int FREQUENCY_OF_CHANGING_PWM = 30000;

class PWMDevice {
public:
	PWMDevice() = default;
	PWMDevice(unsigned char pin1);

	void setPower(unsigned short power);
	uint8_t getPower() const;

	uint8_t getChannel();

	static uint8_t lastChannel;// todo dangerous?

private:
	int8_t channel_;
	uint8_t power_;
};


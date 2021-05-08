#pragma once

#include <Arduino.h>

const int RESOLUTION = 10; //Resolution 8 does not seem to work, 10, 12, 15
const int FREQUENCY_OF_CHANGING_PWM = 30000;


class PWMDevice {
public:
	PWMDevice();
	static void setLastChannel();
	void setup(unsigned char pin1, unsigned char pin2);
	void setPower(uint8_t power);
	uint8_t getChannel();

	~PWMDevice();
	static uint8_t lastChannel = 0;// todo dangerous?

private:
	int8_t channel_;
	uint8_t power_;
};


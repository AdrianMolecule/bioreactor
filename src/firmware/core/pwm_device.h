#pragma once

#include <Arduino.h>



class PWMDevice {
public:
	static constexpr uint8_t MAX_POWER = 100;
	static constexpr uint8_t RESOLUTION = 10; //Resolution 8 does not seem to work, 10, 12, 15
	static constexpr uint8_t FREQUENCY_OF_CHANGING_PWM = 30000;

	PWMDevice() = default;
	PWMDevice(uint8_t pin1);

	void setPower(uint8_t power);
	uint8_t getPower() const;

	uint8_t getChannel();
	void reattachPin(uint8_t pin1);

	static uint8_t lastChannel;// todo dangerous?
private:
	int8_t _channel;
	uint8_t _power;
	uint8_t _pin;
};


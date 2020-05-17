#ifndef CONFIG_H
#define CONFIG_H

#include <driver/adc.h>

namespace config
{
	constexpr char deviceName[] PROGMEM = "bioreactor";	// real domain will be with label ".local", ex "http://bioreactor.local"

	//--------- WiFi access point
	constexpr char AP_ip[] PROGMEM = "192.168.1.1";
	constexpr char AP_gateway[] PROGMEM = "192.168.1.1";
	constexpr char AP_subnet[] PROGMEM = "255.255.255.0";

	//--------- Stepper motor config
	constexpr int motor_steps = 200;
	constexpr int motor_pin_1 = 2;
	constexpr int motor_pin_2 = 4;
	constexpr int motor_pin_3 = 19;
	constexpr int motor_pin_4 = 23;

	//--------- pins
	constexpr unsigned char temp_sensor = 15;
	constexpr adc1_channel_t ph_sensor = ADC1_CHANNEL_3;
	constexpr unsigned char ph_pump_relay = 15;
};

#endif

#ifndef CONFIG_H
#define CONFIG_H

#include <driver/adc.h>
#include "Arduino.h"
namespace config
{
	constexpr char deviceName[] PROGMEM = "bioreactor";	// real domain will be with label ".local", ex "http://bioreactor.local"

	namespace network
	{
		//--------- WiFi access point
		constexpr char AP_ip[] PROGMEM = "192.168.1.1";
		constexpr char AP_gateway[] PROGMEM = "192.168.1.1";
		constexpr char AP_subnet[] PROGMEM = "255.255.255.0";
	}

	//--------- Stepper motor
	namespace motor
	{
		constexpr int steps = 200;
		constexpr int step = 25;
		constexpr int direction = 32;
		constexpr int power = 27;
	}

	namespace HBridge
	{
		constexpr unsigned char power = 13;

		struct input
		{
			unsigned char A;
			unsigned char B;
		};

		constexpr std::array<input, 4> pins{{ {18,16}, {4,15}, {33,23}, {5,14} }};
	}

	constexpr std::array<unsigned char, 2> fet{ 17, 26};

	namespace sensor
	{
		//--------- Temperature
		constexpr unsigned char temp_pin = 19;

		//--------- PH sensor
		constexpr adc1_channel_t ph_adc = ADC1_CHANNEL_3;
	}

	constexpr unsigned char led_pin = 19;
};

#endif

#include "sensor_state.h"

#include <Wire.h>

SensorState::SensorState(adc1_channel_t ph_adc, unsigned char temperature_pin)
	: light{BH1750FVI::k_DevModeContHighRes2},
	  ph{ph_adc}
{
	Wire.begin();
	light.begin();
	temperature.init(temperature_pin);
}

uint32_t SensorState::readLight() {
	const size_t times = 5;
	uint32_t l = 0;
	uint32_t sampleLux = 0;
	for (size_t i = 0; i < times; i++) {
		l += light.GetLightIntensity();
	}
	sampleLux = l / (float)times;
	return sampleLux;
}

std::array<float, 3> SensorState::readTemperature()
{
	std::array<float, 3> readings{0};
	for(uint8_t i = 0; i < temperature.sensorCount() && i < readings.size(); ++i)
	{
		readings[i] = temperature.readCelcius(i);
	}

	return readings;
}

float SensorState::readPH()
{
	return ph.readPH();
}


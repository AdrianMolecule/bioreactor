#include "sensor_state.h"

#include <Wire.h>

SensorState::SensorState(adc1_channel_t ph_adc, unsigned char temperature_pin)
	: _light{BH1750FVI::k_DevModeContHighRes2},
	  _ph{ph_adc}
{
	Wire.begin();
	_light.begin();
	_temp.init(temperature_pin);
}

uint32_t SensorState::readLight() const
{
	const size_t times = 5;
	uint32_t l = 0;
	uint32_t sampleLux = 0;
	for (size_t i = 0; i < times; i++) {
		l += _light.GetLightIntensity();
	}
	sampleLux = l / (float)times;
	return sampleLux;
}

std::array<float, 3> SensorState::readTemperature() const
{
	std::array<float, 3> readings{0};

	{
		readings[0] = _temp.readCelcius();
	}

	return readings;
}

float SensorState::readPH() const
{
	return _ph.readPH();
}


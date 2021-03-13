#pragma once

#include <driver/adc.h>
#include <array>

#include <BH1750FVI.h>
#include "sensors/DS18B20.h"
#include "sensors/PH.h"

class SensorState
{
public:
	struct Readings
	{
		std::array<float, 3> _temp;
		float _ph;
		uint32_t _light;
	};

	SensorState(adc1_channel_t ph_adc, unsigned char temperature_pin);
	std::array<float, 3> readTemperature() const;
	float readPH() const ;
	uint32_t readLight() const ;

private:
	mutable BH1750FVI _light;
	sensor::PH _ph;
	sensor::DS18B20 _temp;
};


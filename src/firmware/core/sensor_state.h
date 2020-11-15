#pragma once

#include <driver/adc.h>
#include <array>

#include <BH1750FVI.h>
#include "sensors/DS18B20.h"
#include "sensors/PH.h"

class SensorState
{
public:
	SensorState(adc1_channel_t ph_adc, unsigned char temperature_pin);
	std::array<float, 3> readTemperature();
	float readPH();
	uint32_t readLight();

private:
	BH1750FVI light;
	sensor::PH ph;
	sensor::DS18B20 temperature;
};


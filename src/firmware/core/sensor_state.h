#pragma once

#include <driver/adc.h>
#include <array>

#include <BH1750FVI.h>
#include <ArduinoJson.h>

#include "sensors/DS18B20.h"
#include "sensors/PH.h"

class SensorState
{
public:
	struct Readings
	{
		void serializeState(JsonObject& state) const
		{
			state["ph"] = _ph;
			state["temp"][0] = _temp[0];
			state["temp"][1] = 0; //sensors->readTemperature()[1];
			state["temp"][2] = 0; //sensors->readTemperature()[2];
			state["light"] = _light;
		};

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


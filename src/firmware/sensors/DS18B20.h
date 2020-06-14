#pragma once

#include <OneWire.h>
#include <DallasTemperature.h>

namespace sensor
{
	class DS18B20
	{
	public:
		DS18B20();

		void init(unsigned dataPin);
		float readCelcius(uint8_t index) const;
		uint8_t sensorCount() const;

	private:
		OneWire dataWire;
		mutable DallasTemperature sensors;
	};

}

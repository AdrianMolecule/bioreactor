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
		float readCelcius() const;

	private:
		OneWire dataWire;
		mutable DallasTemperature sensors;
	};

}

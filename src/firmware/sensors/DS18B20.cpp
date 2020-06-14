#include "DS18B20.h"

using namespace sensor;

DS18B20::DS18B20() : sensors(&dataWire)
{
}

void DS18B20::init(unsigned dataPin)
{
	dataWire.begin(dataPin);
	sensors.begin();
}

float DS18B20::readCelcius(uint8_t index) const
{
	sensors.requestTemperatures();
	return sensors.getTempCByIndex(index);
}

uint8_t DS18B20::sensorCount() const
{
	return sensors.getDS18Count();
}

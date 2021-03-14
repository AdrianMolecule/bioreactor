#include "DS18B20.h"

using namespace sensor;

DS18B20::DS18B20() : sensors(&dataWire)
{
}

void DS18B20::init(unsigned dataPin)
{
	dataWire.begin(dataPin);
	sensors.begin();
	sensors.setResolution(10);
	sensors.getAddress(&address0, 0);
}

float DS18B20::readCelcius() const
{
	sensors.requestTemperatures();
	return sensors.getTempC(&address0);
}

uint8_t DS18B20::sensorCount() const
{
	return sensors.getDS18Count();
}

#include "DS18B20.h"

using namespace sensor;

DS18B20::DS18B20() : sensors(&dataWire)
{
	sensors.begin();
}

void DS18B20::init(unsigned dataPin)
{
	pinMode(dataPin, INPUT_PULLUP);
	dataWire.begin(dataPin);
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

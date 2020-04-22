#ifndef DS18B20_H
#define DS18B20_H

#include "interface.h"

#include <OneWire.h>
#include <DallasTemperature.h>

class DS18B20 : public TempSensor
{
public:
	DS18B20();

	void init(unsigned dataPin) override;
	float readCelcius() const override;

private:
	OneWire dataWire;
	mutable DallasTemperature sensors;
};

#endif

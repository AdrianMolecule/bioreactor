#ifndef TEMP_FACTORY_H
#define TEMP_FACTORY_H

#include <memory>
#include "interface.h"

enum class TempSensors
{
	DS18B20
};

std::unique_ptr<TempSensor> CreateTempSensor(TempSensors instance);

#endif

#include "factory.h"
#include "DS18B20.h"

std::unique_ptr<TempSensor> CreateTempSensor(TempSensors type)
{

	switch(type)
	{
		case TempSensors::DS18B20:
			return std::unique_ptr<TempSensor>( new DS18B20() );
	}

	return nullptr;
}

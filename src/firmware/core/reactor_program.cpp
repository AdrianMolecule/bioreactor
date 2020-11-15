#include "reactor_program.h"
#include "hwinit.h"

void program_run(std::unique_ptr<SensorState>& sensors, std::shared_ptr<ReactorState> reactor)
{
	float target_temp, target_ph;
	getProgramSettings(target_temp, target_ph);

	if(sensors->readTemperature()[0] != target_temp)
	{
		reactor->changeFET(0, true);
	}
	else
	{
		reactor->changeFET(0, false);
	}
}


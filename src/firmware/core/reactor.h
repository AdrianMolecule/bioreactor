#pragma once

#include "actuators.h"
#include "sensor_state.h"

class Reactor
{
public:
	Reactor(const SensorState* sensors, Actuators* act_mgr);

	bool enable();
	bool is_enabled() const;
	bool disable();

	Actuators* get_actuators() const;
	void program_step();

private:
	const SensorState* _sensors;
	Actuators* _act_mgr;
	bool _enabled;
};

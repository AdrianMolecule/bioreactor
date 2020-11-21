#include "core/reactor.h"
#include "misc.h"

Reactor::Reactor(const SensorState* sensors, Actuators* act_mgr) : _sensors{ sensors },
	_act_mgr{ act_mgr },
	_enabled{false}
{

}

bool Reactor::enable()
{
	_enabled = true;
	return true;
}

bool Reactor::is_enabled() const
{
	return _enabled;
}

bool Reactor::disable()
{
	_enabled = false;
	return true;
}

Actuators* Reactor::get_actuators() const
{
	return _act_mgr;
}

void Reactor::program_step()
{
	if( !is_enabled() )
		return;

	float target_temp, target_ph;
	getProgramSettings(target_temp, target_ph);

	if(_sensors->readTemperature()[0] != target_temp)
	{
		_act_mgr->changeFET(0, true);
	}
	else
	{
		_act_mgr->changeFET(0, false);
	}
}


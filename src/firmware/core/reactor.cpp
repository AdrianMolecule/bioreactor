#include "reactor.h"

#include <Preferences.h>

Reactor::Reactor(const SensorState* sensors, Actuators* act_mgr) : _program{ act_mgr },
	_sensors{ sensors },
	_act_mgr{ act_mgr },
	_program_enabled{false}
{
	build_program_list();
}

bool Reactor::program_enabled() const
{
	return _program_enabled;
}
uint8_t Reactor::program_active() const
{
	return _program_active;
}

Actuators* Reactor::get_actuators() const
{
	return _act_mgr;
}

void Reactor::program_step()
{
	if( !program_enabled() )
	{
		delay(500);
		return;
	}

	const Reactor::ProgramSettings& active = programs[_program_active];

	//Serial.printf("program_step: name %s temp %f\n", active.name.c_str(), active.temp);
	_act_mgr->changeMotor(true);
	_act_mgr->runMotor();

	if(_sensors->readTemperature()[0] < active.temp)
	{
		_act_mgr->changeFET(0, true);
	}
	else
	{
		_act_mgr->changeFET(0, false);
	}
}

void Reactor::build_program_list()
{
	Preferences preferences;
	preferences.begin("program", true);
	uint8_t count = preferences.getUChar("count");
	_program_active = preferences.getUChar("active");
	_program_enabled = preferences.getBool("enabled");
	preferences.end();

	programs.reserve(count);
	for(size_t i=0; i < count; ++i)
	{
		programs.push_back( read_single_program(i) );
	}
}

const std::vector<Reactor::ProgramSettings>& Reactor::read_programs_list() const
{
	return programs;
}

Reactor::ProgramSettings Reactor::read_single_program(uint8_t id)
{
	ProgramSettings settings;

	if(programs.size() > id)
	{
		return programs[id];
	}

	char mem_namespace[16];
	sprintf(mem_namespace, "program/%d", id);

	Serial.printf("read_single_program: id %d\n", id);

	Preferences preferences;
	preferences.begin(mem_namespace, true);
	settings.temp = preferences.getFloat("temp", .0);
	settings.ph = preferences.getFloat("ph", .0);
	settings.name = preferences.getString("name");
	settings.id = id;
	preferences.end();

	return settings;
}

void Reactor::save_program(ProgramSettings& settings, bool enabled, bool is_new)
{
	_program_enabled = enabled;

	if(!_program_enabled)
	{
		_act_mgr->shutdown();
	}

	if(is_new)
	{
		settings.id = programs.size();
	}

	char mem_namespace[16];
	sprintf(mem_namespace, "program/%d", settings.id);

	Serial.printf("save_program: id %d programs.size = %d\n", settings.id, programs.size());

	Preferences preferences;
	preferences.begin(mem_namespace);
	preferences.putFloat("temp", settings.temp);
	preferences.putFloat("ph", settings.ph);
	preferences.putString("name", settings.name);
	preferences.end();

	update_program_list(settings, enabled);
}

void Reactor::update_program_list(ProgramSettings& settings, bool enabled)
{
	Preferences preferences;
	preferences.begin("program");

	//update counter and programs storage if new id is detected
	if(programs.size() <= settings.id)
	{
		Serial.printf("update_program_list: update counter till %d\n", settings.id + 1);
		programs.push_back(settings);
		preferences.putUChar("count", settings.id + 1);
	}
	else
	{
		programs[settings.id] = settings;
	}

	preferences.putUChar("active", settings.id);
	preferences.putBool("enabled", enabled);
	preferences.end();

	_program_active = settings.id;
	_program_enabled = enabled;
}


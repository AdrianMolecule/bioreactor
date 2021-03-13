#pragma once

#include <arduino-timer.h>
#include <deque>

#include "actuators.h"
#include "sensor_state.h"
#include "program/basic.h"

class Reactor
{
public:
	Reactor(const SensorState* sensors, Actuators* act_mgr, unsigned short sensor_read_rate);

	bool program_enabled() const;
	uint8_t program_active() const;

	Actuators* get_actuators() const;
	void program_step();

	struct ProgramSettings
	{
		String name;
		float temp;
		float ph;
		uint8_t id;
	};

	void save_program(ProgramSettings& settings, bool enabled, bool is_new = false);
	const std::vector<ProgramSettings>& read_programs_list() const;

	void sensor_reading();
	void schedule_routines(unsigned short sensor_rate_sec);

	std::deque<SensorState::Readings> _sensor_data;

private:
	void build_program_list();
	ProgramSettings read_single_program(uint8_t number);
	void update_program_list(ProgramSettings& settings, bool enabled);

	std::vector<ProgramSettings> programs;
	Program::Basic _program;

	Timer<2, millis, Reactor*> timer;
	const SensorState* _sensors;


	Actuators* _act_mgr;
	uint8_t _program_active;
	bool _program_enabled;
};

#pragma once

#include <arduino-timer.h>
#include <deque>
#include <ArduinoJson.h>

#include "actuators.h"
#include "sensor_state.h"
#include "program/basic.h"
#include "config.h"

#include "SPIFFS.h"
class SensorDataCache
{
	std::deque<SensorState::Readings> file_cache;
	bool started { false };
	static constexpr size_t FILE_CACHE_SIZE = 100;

public:
	void push_data(const SensorState::Readings& data)
	{
		if( started )
			file_cache.push_back( data );

		if(file_cache.size() > FILE_CACHE_SIZE)
		{
			dump_file_cache();
		}
	}

	void start()
	{
		if(!config::SENSOR_HISTORY_ENABLED || started)
			return;

		file_cache.clear();
		File current = SPIFFS.open("/currentrun", FILE_WRITE);
		current.close();
		started = true;
	}

	void stop()
	{
		if(!started)
			return;

		dump_file_cache();

		SPIFFS.remove("/lastrun");
		SPIFFS.rename("/currentrun", "/lastrun");
		started = false;
	}



	void dump_file_cache()
	{
		if(!started)
			return;

		File file = SPIFFS.open("/currentrun", FILE_APPEND);
		for(const auto& sensor_data : file_cache)
		{
			file.write((const uint8_t*)&sensor_data, sizeof sensor_data);
		}
		Serial.println("Reactor::dump_file_cache: currentrun file is updated");
		file.close();

		file_cache.clear();
	}

};

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


	void serializeState(JsonObject&) const;


	struct SensorReadings
	{
		//uint8_t program_id;
		time_t start_time;
		std::deque<SensorState::Readings> data;

		bool new_data_available;
	} _sensor_data;

private:
	void build_program_list();
	ProgramSettings read_single_program(uint8_t number);
	void update_program_list(ProgramSettings& settings, bool enabled);

	std::vector<ProgramSettings> programs;
	Program::Basic _program;

	Timer<2, millis, Reactor*> scheduler_timer;
	const SensorState* _sensors;


	Actuators* _act_mgr;
	uint8_t _program_active;
	bool _program_enabled;
	unsigned short _sensor_read_rate;

	SensorDataCache storage;

	static constexpr size_t UI_HISTORY_SIZE = 300;
};

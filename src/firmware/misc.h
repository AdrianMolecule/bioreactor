#ifndef HWINIT_H
#define HWINIT_H

#include <vector>

#include "core/sensor_state.h"
#include "core/reactor.h"


class String;

bool WiFiConnect();
std::vector<String> WiFiNetworks();

void getWifiSettings(String& ssid, String& password);
void saveWifiSettings(String&& ssid, String&& password);

void getServerSettings(unsigned short &sensor_read_rate);
void saveServerSettings(const unsigned short sensor_rate);

void resetMemory();

//---- state to Json converter
String serializeState(const Reactor* reactor_mgr, const SensorState::Readings& sensor_data, time_t timestamp = 0);
void dumpMemoryStatistic();
void dumpFlashStatistic();

void initBoardTime();

#endif

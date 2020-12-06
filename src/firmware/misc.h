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
void resetMemory();

//---- state to Json converter
String serializeState(const SensorState* sensors, const Reactor* reactor_mgr);
void dumpMemoryStatistic();

#endif

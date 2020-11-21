#ifndef HWINIT_H
#define HWINIT_H

#include <memory>

#include "core/sensor_state.h"
#include "core/reactor_state.h"
#include <vector>

class String;

bool WiFiConnect();
std::vector<String> WiFiNetworks();

void getWifiSettings(String& ssid, String& password);
void saveWifiSettings(String&& ssid, String&& password);

void getProgramSettings(float& temperature, float& ph);
void saveProgramSettings(String&& temperature, String&& ph);

//---- state to Json converter
String serializeState(std::unique_ptr<SensorState>& sensors, std::shared_ptr<ReactorState> reactor);

#endif

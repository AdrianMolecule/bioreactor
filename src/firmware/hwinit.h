#ifndef HWINIT_H
#define HWINIT_H

#include <vector>

class String;

bool WiFiConnect();
std::vector<String> WiFiNetworks();

void getWifiSettings(String& ssid, String& password);
void saveWifiSettings(String&& ssid, String&& password);

void getProgramSettings(float& temperature, float& ph);
void saveProgramSettings(const float& temperature, const float& ph);

#endif

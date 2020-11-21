#include <misc.h>

#include "config.h"
#include "core/sensor_state.h"
#include "core/reactor_state.h"

#include "lwip/inet.h"
#include <WiFi.h>
#include <Preferences.h>
#include <ArduinoJson.h>

bool WiFiConnect()
{
	WiFi.mode(WIFI_AP_STA);
/*
	if(!WiFi.softAPConfig(
			inet_addr(config::AP_ip),
			inet_addr(config::AP_gateway),
			inet_addr(config::AP_subnet)
			)
		)
	{
		return false;
	}
*/
	String APName = String(config::deviceName) + "-" + WiFi.macAddress();
	WiFi.softAP(APName.c_str());
	delay(100);

	String ssid, password;
	getWifiSettings(ssid, password);

	if(!ssid.isEmpty())
	{
		for(size_t tries = 0; tries < 10 && WiFi.status() != WL_CONNECTED; ++tries)
		{
			WiFi.begin(ssid.c_str(), password.c_str());
			delay(1000);
			Serial.println("Connecting to '" + ssid + "'");
		}

		if(WiFi.status() != WL_CONNECTED)
		{
			Serial.println("Failed connect to '" + ssid + "'");
			WiFi.disconnect(true);
		}
	}
	return true;
}

std::vector<String> WiFiNetworks()
{
	size_t networks = WiFi.scanNetworks();

	std::vector<String> result(networks);

	for (size_t i = 0; i < networks; ++i)
	{
		result.push_back(WiFi.SSID(i));
		//WiFi.RSSI(i)
		//WiFi.encryptionType(i) == WIFI_AUTH_OPEN
	}
	return result;
}

void getWifiSettings(String& ssid, String& password)
{
	Preferences preferences;
	preferences.begin("wifi", true);
	ssid = preferences.getString("ssid", "");
	password = preferences.getString("pass", "");
	preferences.end();
}

void saveWifiSettings(String&& ssid, String&& password)
{
	Preferences preferences;
	preferences.begin("wifi");
	if(!ssid.isEmpty())
		preferences.putString("ssid", ssid);
	if(!password.isEmpty())
		preferences.putString("pass", password);
	preferences.end();
}

void getProgramSettings(float& temperature, float& ph)
{
	Preferences preferences;
	preferences.begin("program", true);
	temperature = preferences.getFloat("temp", .0);
	ph = preferences.getFloat("ph", .0);
	preferences.end();
}

void saveProgramSettings(String&& temperature, String&& ph)
{
	Preferences preferences;
	preferences.begin("program");
	if(!temperature.isEmpty())
		preferences.putFloat("temp", temperature.toFloat());
	if(!ph.isEmpty())
		preferences.putFloat("ph", ph.toFloat());
	preferences.end();
}

String serializeState(std::unique_ptr<SensorState>& sensors, std::shared_ptr<ReactorState> reactor)
{
	static StaticJsonDocument<300> state;
	state["ph"] = sensors->readPH();
	state["temp"][0] = sensors->readTemperature()[0];
	state["temp"][1] = 0; //sensors->readTemperature()[1];
	state["temp"][2] = 0; //sensors->readTemperature()[2];
	state["light"] = sensors->readLight();

	for(size_t i = 0; i < config::fet.size(); ++i)
		state["fet"][i] = reactor->read().fet[i];

	for(size_t i = 0; i < config::HBridge::pins.size(); ++i)
	{
		state["hbridge"][i] = bridgeStateConvert(reactor->read().hbridge[i]);
	}
	state["led"] = reactor->read().led;
	state["motor"] = reactor->read().motor;
	state["reactor_enabled"] = reactor->is_enabled();

	String data;
	serializeJson(state, data);
	return data;
}

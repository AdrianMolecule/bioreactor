#include <misc.h>

#include "config.h"
#include "core/sensor_state.h"
#include "lwip/inet.h"
#include <WiFi.h>
#include <Preferences.h>
#include <ArduinoJson.h>
#include <core/actuators.h>
#include <esp_heap_caps.h>
#include <esp_sntp.h>

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
			delay(5000);
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

void getServerSettings(unsigned short &sensor_read_rate)
{
	Preferences preferences;
	preferences.begin("server", true);
	sensor_read_rate = preferences.getShort("sensor_rate",5);
	preferences.end();
}

void saveServerSettings(const unsigned short sensor_rate)
{
	Preferences preferences;
	preferences.begin("server");
	preferences.putShort("sensor_rate", sensor_rate);
	preferences.end();
}

String serializeState(const Reactor* reactor_mgr, const SensorState::Readings& sensor_data, time_t timestamp)
{
	static StaticJsonDocument<500> state;
	state.clear();

	JsonObject object = state.to<JsonObject>();
	sensor_data.serializeState(object);
	reactor_mgr->serializeState(object);

	if(timestamp)
		object["timestamp"] = timestamp;

	String data;
	serializeJson(state, data);

	return data;
}

void resetMemory()
{
	Preferences preferences;
	preferences.begin("program");
	uint8_t count = preferences.getUChar("count");
	preferences.clear();
	preferences.end();


	for(size_t i=0; i < count; ++i)
	{
		char mem_namespace[16];
		sprintf(mem_namespace, "program/%d", i);

		Serial.printf("Clear %s\n", mem_namespace);

		preferences.begin(mem_namespace);
		preferences.clear();
		preferences.end();
	}
	Serial.printf("found %d programs\n", count);
	//preferences.begin("wifi");
	//preferences.clear();
}

void dumpMemoryStatistic()
{
	//heap_caps_print_heap_info(MALLOC_CAP_DEFAULT);
	heap_caps_print_heap_info(MALLOC_CAP_32BIT);
}


void initBoardTime()
{
	sntp_setoperatingmode(SNTP_OPMODE_POLL);
	sntp_setservername(0, "pool.ntp.org");
	sntp_init();

	// Set timezone to Ottawa
	setenv("TZ", "UTC", 1);
	tzset();
}


#include <ESPmDNS.h>

#include "display/factory.h"
#include "httpserver.h"

#include "config.h"
#include "core/sensor_state.h"
#include "core/actuators.h"
#include "core/reactor.h"
#include "misc.h"

std::unique_ptr<Display> display;

Actuators* act_mgr;
Reactor* reactor_mgr;

HTTPServer server;

void setup() {
	Serial.begin(9600);

	//resetMemory();

	display = CreateDisplay(Displays::ST7735);
	display->init();



	if(!WiFiConnect())
	{
		Serial.println("Failed to init AP mode");
	}



	String ip = "APIP: " + WiFi.softAPIP().toString() + "\n" +
			"WFIP: " + WiFi.localIP().toString();

	Serial.println(ip);

	if (!MDNS.begin(config::deviceName))
	{
		display->print(ip.c_str());
	}
	else
	{
		MDNS.addService("http","tcp",80);
		String host = config::deviceName;
		host = "http://" + host + ".local";
		display->print(host);
	}

	initBoardTime();

	//disabling buzzer
	digitalWrite(config::buzzer_pin, LOW);

	SensorState *sensors = new SensorState(config::sensor::ph_adc, config::sensor::temp_pin);
	act_mgr = new Actuators();

	unsigned short sensor_read_rate = -1;
	getServerSettings(sensor_read_rate);
	reactor_mgr = new Reactor(sensors, act_mgr, sensor_read_rate);
	server.init(reactor_mgr);
}

void loop()
{
	reactor_mgr->program_step();

	//Serial.println(data);
	//dumpMemoryStatistic();

	server.loop();

	delay(10);
}


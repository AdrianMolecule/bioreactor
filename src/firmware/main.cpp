#include <ESPmDNS.h>

#include "display/factory.h"
#include "httpserver.h"

#include "config.h"
#include "core/sensor_state.h"
#include "core/reactor_state.h"
#include "core/reactor_program.h"

#include <misc.h>

std::unique_ptr<Display> display;
std::unique_ptr<SensorState> sensors;
std::shared_ptr<ReactorState> reactor;

HTTPServer server;

void setup() {
	Serial.begin(9600);

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


	sensors.reset(new SensorState(config::sensor::ph_adc, config::sensor::temp_pin));
	reactor.reset(new ReactorState());
	server.init(reactor);
	//adc1_config_width(ADC_WIDTH_BIT_12);

}

void loop() {

	digitalWrite(config::motor::direction,HIGH); //Enables the motor to move in a perticular direction
	// for one full rotation required 200 pulses
	for(int x = 0; x < 900; x++){
	  digitalWrite(config::motor::step,HIGH);
	  delayMicroseconds(500);
	  digitalWrite(config::motor::step,LOW);
	  delayMicroseconds(500);
	}

	program_run(sensors, reactor);

	String data = serializeState(sensors, reactor);

	//Serial.println(data);
	server.loop();
	server.sendWebSockData(data);

	//delay(100);
}

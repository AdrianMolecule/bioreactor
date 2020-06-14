#include <ESPmDNS.h>

#include "display/factory.h"
#include "httpserver.h"

#include "hwinit.h"
#include "config.h"
#include "sensor_state.h"
#include "reactor_state.h"

#include <ArduinoJson.h>

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
/*
	digitalWrite(config::motor::direction,HIGH); //Enables the motor to move in a perticular direction
	// for one full rotation required 200 pulses
	for(int x = 0; x < 900; x++){
	  digitalWrite(config::motor::step,HIGH);
	  delayMicroseconds(500);
	  digitalWrite(config::motor::step,LOW);
	  delayMicroseconds(500);
	}
*/


	float tempValue = sensors->readTemperature()[0];

	if(tempValue < 27.0)
		reactor->changeFET(0, true);
	else
		reactor->changeFET(0, false);

	static StaticJsonDocument<300> state;
	state["ph"] = sensors->readPH();
	state["temp"][0] = sensors->readTemperature()[0];
	state["temp"][1] = sensors->readTemperature()[1];
	state["temp"][2] = sensors->readTemperature()[2];
	state["light"] = sensors->readLight();

	for(size_t i = 0; i < config::fet.size(); ++i)
		state["fet"][i] = reactor->read().fet[i];

	for(size_t i = 0; i < config::HBridge::pins.size(); ++i)
	{
		state["hbridge"][i] = bridgeStateConvert(reactor->read().hbridge[i]);
	}
	state["led"] = reactor->read().led;

	String data;
	serializeJson(state, data);

	Serial.println(data);
	server.loop();
	server.sendWebSockData(data);


	//delay(100);
}

#include <ESPmDNS.h>

#include "display/factory.h"
#include "httpserver.h"

#include <Stepper.h>

#include "hwinit.h"
#include "config.h"

#include "sensors/DS18B20.h"
#include "sensors/PH.h"

std::unique_ptr<Display> display;
std::unique_ptr<sensor::PH> phsensor;
std::unique_ptr<sensor::DS18B20> tempsensor;
HTTPServer server;
Stepper stepper(config::motor_steps,
					config::motor_pin_1,
					config::motor_pin_2,
					config::motor_pin_3,
					config::motor_pin_4
				);

void setup() {
	Serial.begin(9600);

	display = CreateDisplay(Displays::ST7735);
	display->init();

	tempsensor.reset(new sensor::DS18B20());
	tempsensor->init(config::temp_sensor);
	phsensor.reset(new sensor::PH(config::ph_sensor));

	if(!WiFiConnect())
	{
		Serial.println("Failed to init AP mode");
	}

	server.init();

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

	//stepper.setSpeed(170);
	pinMode(config::ph_sensor, OUTPUT);
	digitalWrite(config::ph_sensor, LOW);
	//ph.begin();

	//adc1_config_width(ADC_WIDTH_BIT_12);
}

void loop() {

//	stepper.step(100);
	float phValue = phsensor->readPH();
	float tempValue = tempsensor->readCelcius();

	if(phValue < 5)
		digitalWrite(config::ph_pump_relay, HIGH);
	else if(phValue > 6.0)
		digitalWrite(config::ph_pump_relay, LOW);

	String data("{\"ph\":\"");
	data += phValue;
	data += "\", \"temp\":\"";
	data += tempValue;
	data += "\"}";

	Serial.println(data);
	server.loop();
	server.sendWebSockData(data);

	delay(1000);
}

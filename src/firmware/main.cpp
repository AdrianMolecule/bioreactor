#include <ESPmDNS.h>

#include "display/factory.h"
#include "temp/factory.h"
#include "httpserver.h"

#include <Stepper.h>

#include "hwinit.h"
#include "config.h"

std::unique_ptr<Display> display;
std::unique_ptr<TempSensor> temp;
HTTPServer server;
Stepper stepper(200,2,4,19,23);

#include "ph.h"

DFRobot_ESP_PH ph;
#define ESPADC 4096.0   //the esp Analog Digital Convertion value
#define ESPVOLTAGE 3300 //the esp voltage supply value
#define PH_PIN 15  //the esp gpio data pin number
float voltage, phValue = 7, temperature = 20;

#include <driver/adc.h>
void setup() {
	Serial.begin(9600);

	display = CreateDisplay(Displays::ST7735);
	display->init();

	//temp = CreateTempSensor(TempSensors::DS18B20);
	//temp->init(15);

	WiFiConnect();

	server.init();

	if (1 || !MDNS.begin(config::domainName))
	{
		String out = "IP: " + WiFi.localIP().toString();
		display->print(out.c_str());
	}
	else
	{
		String host = config::domainName;
		host = "http://" + host + ".local";
		display->print(host);
	}

	//stepper.setSpeed(170);
	pinMode(PH_PIN, OUTPUT);
	digitalWrite(PH_PIN, LOW);
	//ph.begin();

	voltage = 0;
	adc1_config_width(ADC_WIDTH_BIT_12);
	//adc1_config_channel_atten(ADC1_CHANNEL_0,ADC_ATTEN_DB_0);
	//adc1_config_channel_atten(ADC1_CHANNEL_3,ADC_ATTEN_DB_0);
}



void loop() {

//	stepper.step(100);

	/*
	float tem = temp->readCelcius();
	String data(tem);

	server.loop();
	server.sendWebSockData(data);
	delay(1000);
	*/


	static unsigned long timepoint = millis();
	 if (millis() - timepoint > 1000U) //time interval: 1s
	 {
	  timepoint = millis();
	  //voltage = rawPinValue / esp32ADC * esp32Vin

	  voltage = adc1_get_raw(ADC1_CHANNEL_3) / ESPADC * ESPVOLTAGE; // read the voltage
	  Serial.print("voltage: ");
	  Serial.print(voltage);

	  //temperature = readTemperature();  // read your temperature sensor to execute temperature compensation

	  phValue = ph.readPH(voltage, temperature); // convert voltage to pH with temperature compensation
	  Serial.print(" pH:");
	  Serial.println(phValue, 4);
	 }
	 ph.calibration(voltage, temperature);
	 if(phValue < 5)
		 digitalWrite(PH_PIN, HIGH);
	 else if(phValue > 6.0)
		 digitalWrite(PH_PIN, LOW);


	 String data(phValue);
	server.loop();
	server.sendWebSockData(data);
	delay(1000);
}


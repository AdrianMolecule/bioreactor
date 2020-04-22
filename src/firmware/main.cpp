#include <WiFi.h>
#include <ESPmDNS.h>

#include "display/factory.h"
#include "temp/factory.h"
#include "httpserver.h"


const char* ssid = "natashka";
const char* password = "12345678";
IPAddress local_ip(192,168,1,1);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,1);
const char* host = "bioreactor";

std::unique_ptr<Display> display;
std::unique_ptr<TempSensor> temp;

HTTPServer server;

void setup() {
  Serial.begin(9600);

  display = CreateDisplay(Displays::ST7735);
  display->init();

  //WiFi.softAP(ssid, password);
  //WiFi.softAPConfig(local_ip, gateway, subnet);
  temp = CreateTempSensor(TempSensors::DS18B20);
  temp->init(15);

    while (WiFi.status() != WL_CONNECTED) {
        WiFi.begin(ssid, password);
    delay(1000);
    display->print("Connecting to WiFi..");
  }
    String out = "IP: " + WiFi.localIP().toString();
    display->print(out.c_str());

  Serial.printf("HTTP server started on %s\n", WiFi.localIP());

  server.init();
  MDNS.begin(host);
}

void loop() {
	float tem = temp->readCelcius();
	Serial.println(tem);
	String data(tem);

	server.loop();
	server.sendWebSockData(data);
	delay(1000);
}


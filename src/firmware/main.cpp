#include "SPIFFS.h"
#include <WiFi.h>
#include <WebServer.h>
#include "display/factory.h"


const char* ssid = "natashka";
const char* password = "12345678";
IPAddress local_ip(192,168,1,1);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,1);
std::unique_ptr<Display> display;

WebServer server(80);
void handle_OnConnect();

void setup() {
  Serial.begin(9600);

  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  File file = SPIFFS.open("/index.html");
  if(!file){
    Serial.println("Failed to open file for reading");
    return;
  }

  Serial.println("File Content:");
  /*while(file.available()){
    //Serial.write(file.read());
  }
  file.close();
	*/

  display = CreateDisplay(Displays::ST7735);
  display->init();

  //WiFi.softAP(ssid, password);
  //WiFi.softAPConfig(local_ip, gateway, subnet);


    while (WiFi.status() != WL_CONNECTED) {
        WiFi.begin(ssid, password);
    delay(1000);
    display->print("Connecting to WiFi..");
  }
    String out = "IP: " + WiFi.localIP().toString();
    display->print(out.c_str());

  server.on("/", handle_OnConnect);
  server.begin();
  Serial.printf("HTTP server started on %s\n", WiFi.localIP());

}

void loop() {
  server.handleClient();
}

void handle_OnConnect() {
  Serial.println("connected");
  File file = SPIFFS.open("/index.html");

  server.send(200, "text/html", file.readString());
  file.close();
}

#include "SPIFFS.h"
#include <WiFi.h>
#include <WebServer.h>
#include <Adafruit_ST7735.h>
#include "display.h"


const char* ssid = "natashka";
const char* password = "12345678";
IPAddress local_ip(192,168,1,1);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,1);

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


  display_init();


  //WiFi.softAP(ssid, password);
  //WiFi.softAPConfig(local_ip, gateway, subnet);
  WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    display_text("Connecting to WiFi..", 10, 10, 2);
  }
    String out = "IP: " + WiFi.localIP().toString();
    display_text(out.c_str(), 10, 10, 2);

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

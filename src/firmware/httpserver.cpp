#include "httpserver.h"

#include <functional>
#include "SPIFFS.h"
#include "hwinit.h"

#include "reactor_state.h"

using namespace std::placeholders;

HTTPServer::HTTPServer(uint16_t HTTPPort, uint16_t webSocketPort)
: WSServer(webSocketPort),
  webServer(HTTPPort)
{

}

void HTTPServer::init(std::shared_ptr<ReactorState> reactor)
{
	reactorState = reactor;

	webServer.on("/", std::bind(&HTTPServer::onHTTPConnect, this));
	webServer.on("/settings", std::bind(&HTTPServer::onSettings, this));
	webServer.on("/program", std::bind(&HTTPServer::onProgram, this));
	webServer.on("/header.html", std::bind(&HTTPServer::onFile, this));

	webServer.begin();
	WSServer.begin();
	WSServer.onEvent(std::bind(&HTTPServer::onWSEvent, this, _1, _2, _3, _4));

	  if(!SPIFFS.begin(true)){
	    Serial.println("An Error has occurred while mounting SPIFFS");
	    return;
	  }
}

void HTTPServer::loop()
{
	webServer.handleClient();
	WSServer.loop();
}

void HTTPServer::sendWebSockData(String data)
{
	for(uint8_t client : WSConnections)
	{
		WSServer.sendTXT(client, data);
	}
}

void HTTPServer::onWSEvent(uint8_t num,
                      WStype_t type,
                      uint8_t * payload,
                      size_t length)
{

  switch(type)
  {
    case WStype_DISCONNECTED:
    	WSConnections.erase(num);
      Serial.printf("[%u] Disconnected!\n", num);
      break;
    case WStype_CONNECTED:
      {
    	  WSConnections.insert(num);
        IPAddress ip { WSServer.remoteIP(num) };
        Serial.printf("[%u] Connection from ", num);
        Serial.println(ip.toString());
      }
      break;
    case WStype_TEXT:
      Serial.printf("[%u] Text: %s\n", num, payload);
      break;
    default:
      break;
  }
}

void HTTPServer::onHTTPConnect()
{
	if(webServer.method() == HTTPMethod::HTTP_POST)
	{
		if( webServer.arg("fet1") == "true" )
			reactorState->changeFET(0, true);
		else
			reactorState->changeFET(0, false);

		if( webServer.arg("fet2") == "true" )
			reactorState->changeFET(1, true);
		else
			reactorState->changeFET(1, false);

		reactorState->changeHBridge(0, bridgeStateConvert(webServer.arg("hbridge1")));
		reactorState->changeHBridge(1, bridgeStateConvert(webServer.arg("hbridge2")));
		reactorState->changeHBridge(2, bridgeStateConvert(webServer.arg("hbridge3")));
		reactorState->changeHBridge(3, bridgeStateConvert(webServer.arg("hbridge4")));

		if( webServer.arg("led") == "true" )
			reactorState->changeLED(true);
		else
			reactorState->changeLED(false);
	}

	Serial.println("connected " + webServer.uri());
	File file = SPIFFS.open("/index.html");

	webServer.send(200, "text/html", file.readString());
	file.close();
}

void HTTPServer::onSettings()
{
	if(webServer.method() == HTTPMethod::HTTP_POST)
	{
		saveWifiSettings(webServer.arg("ssid"), webServer.arg("pass"));
	}

	String ssid, password;
	getWifiSettings(ssid, password);

	File file = SPIFFS.open("/settings.html");

	String content(file.size() + ssid.length());
	content = file.readString();
	content.replace("##ssid##", ssid);

	webServer.send(200, "text/html", std::move(content));
	file.close();


}

void HTTPServer::onProgram()
{
	if(webServer.method() == HTTPMethod::HTTP_POST)
	{
		saveProgramSettings(webServer.arg("temperature").toFloat(), webServer.arg("ph").toFloat());
	}

	float temperature, ph;
	getProgramSettings(temperature, ph);
	String tempStr(temperature, 4);
	String phStr(ph, 4);

	Serial.println("Temp before send is " + tempStr);

	File file = SPIFFS.open("/program.html");

	String content(file.size() + tempStr.length() + phStr.length());
	content = file.readString();
	content.replace("##temp##", tempStr);
	content.replace("##phlevel##", phStr);

	webServer.send(200, "text/html", std::move(content));
	file.close();


}

void HTTPServer::onFile()
{
	File file = SPIFFS.open("/header.html");

	webServer.send(200, "text/html", file.readString());
	file.close();
}

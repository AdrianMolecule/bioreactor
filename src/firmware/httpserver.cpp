#include <misc.h>
#include "httpserver.h"

#include <functional>
#include <Update.h>

#include "core/reactor_state.h"

using namespace std::placeholders;

HTTPServer::HTTPServer(uint16_t HTTPPort, uint16_t webSocketPort)
: WSServer(webSocketPort),
  webServer(HTTPPort)
{

}

void HTTPServer::init(std::shared_ptr<ReactorState> reactor)
{
	reactorState = reactor;

	webServer.on("/", std::bind(&HTTPServer::onMain, this));

	webServer.on("/settings", std::bind(&HTTPServer::onSettings, this));

	webServer.on("/firmware", HTTP_ANY,
			std::bind(
					&HTTPServer::responseWithFile,
					this,
					"/dummy.html",
					html_variables({{"##server_message##", "Reboot the board to apply changes"}})
			),
			std::bind(&HTTPServer::onFirmwareUpload, this)
	);

	webServer.on("/program", std::bind(&HTTPServer::onProgram, this));

	webServer.on("/upload", HTTP_ANY,
			std::bind(
					&HTTPServer::responseWithFile,
					this,
					"/dummy.html",
					html_variables({{"##server_message##", "Successfully uploaded"}})
				),
			std::bind(&HTTPServer::handleFileUpload, this)
	);

	if(!SPIFFS.begin(true))
	{
		Serial.println("An Error has occurred while mounting SPIFFS");
		return;
	}

	webServer.serveStatic("/header.html", SPIFFS, "/header.html");
	webServer.serveStatic("/footer.html", SPIFFS, "/footer.html");
	webServer.serveStatic("/dygraph.min.css", SPIFFS, "/dygraph.min.css");
	webServer.serveStatic("/dygraph.min.js", SPIFFS, "/dygraph.min.js");

	webServer.begin();
	WSServer.begin();
	WSServer.onEvent(std::bind(&HTTPServer::onWSEvent, this, _1, _2, _3, _4));
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

void HTTPServer::onMain()
{
	if(webServer.method() == HTTPMethod::HTTP_POST)
	{
		if( webServer.arg("fet1") == "on" )
			reactorState->changeFET(0, true);
		else
			reactorState->changeFET(0, false);

		if( webServer.arg("fet2") == "on" )
			reactorState->changeFET(1, true);
		else
			reactorState->changeFET(1, false);

		reactorState->changeHBridge(0, bridgeStateConvert(webServer.arg("hbridge1")));
		reactorState->changeHBridge(1, bridgeStateConvert(webServer.arg("hbridge2")));
		reactorState->changeHBridge(2, bridgeStateConvert(webServer.arg("hbridge3")));
		reactorState->changeHBridge(3, bridgeStateConvert(webServer.arg("hbridge4")));

		if( webServer.arg("led") == "on" )
			reactorState->changeLED(true);
		else
			reactorState->changeLED(false);

		if( webServer.arg("motor") == "on" )
			reactorState->changeMotor(true);
		else
			reactorState->changeMotor(false);
	}

	Serial.println("connected " + webServer.uri());
	responseWithFile("/index.html", {});
}

void HTTPServer::onSettings()
{
	if(webServer.method() == HTTPMethod::HTTP_POST)
	{
		saveWifiSettings(webServer.arg("ssid"), webServer.arg("pass"));
	}

	String ssid, password;
	getWifiSettings(ssid, password);

	html_variables data;
	data["##ssid##"] = ssid;

	responseWithFile("/settings.html", data);
}

void HTTPServer::onProgram()
{
	if(webServer.method() == HTTPMethod::HTTP_POST)
	{
		saveProgramSettings(webServer.arg("temperature"), webServer.arg("ph"));
		if(reactorState->is_enabled())
			reactorState->disable();
		else
			reactorState->enable();
	}

	float temperature, ph;
	getProgramSettings(temperature, ph);

	html_variables data;
	data["##temp##"] = String(temperature, 2);
	data["##phlevel##"] = String(ph, 2);
	data["##is_enabled##"] = String(reactorState->is_enabled());
	responseWithFile("/program.html", data);
}

void HTTPServer::onFirmwareUpload()
{
	HTTPUpload& upload = webServer.upload();

	if (upload.status == UPLOAD_FILE_START)
	{
	  Serial.printf("Update: %d\n", upload.currentSize);
	  if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { //start with max available size
		Update.printError(Serial);
	  }
	} else if (upload.status == UPLOAD_FILE_WRITE) {
	      /* flashing firmware to ESP*/
	      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
	        Update.printError(Serial);
	      }
	    } else if (upload.status == UPLOAD_FILE_END) {
	      if (Update.end(true)) { //true to set the size to the current progress
	        Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
	        //ESP.restart();
	      } else {
	        Update.printError(Serial);
	      }
	    }
}


void HTTPServer::handleFileUpload() {
  /*if (!fsOK) {
    return replyServerError(FPSTR(FS_INIT_ERROR));
  }

  if (server.uri() != "/edit") {
    return;
  }
  */
  HTTPUpload& upload = webServer.upload();

  if (upload.status == UPLOAD_FILE_START) {
    String filename = upload.filename;
    // Make sure paths always start with "/"
    if (!filename.startsWith("/")) {
      filename = "/" + filename;
    }
    uploadFile = SPIFFS.open(filename, "w");
    if (!uploadFile) {
    	Serial.println("CREATE FAILED");
      return;
    }

    Serial.println(String("Upload: START, filename: ") + filename);
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    if (uploadFile) {
      size_t bytesWritten = uploadFile.write(upload.buf, upload.currentSize);
      if (bytesWritten != upload.currentSize) {
        Serial.println("WRITE FAILED");
        return;
      }
    }
    Serial.println(String("Upload: WRITE, Bytes: ") + upload.currentSize);
  } else if (upload.status == UPLOAD_FILE_END) {
    if (uploadFile) {
      uploadFile.close();
    }
    Serial.println(String("Upload: END, Size: ") + upload.totalSize);
  }
}

void HTTPServer::responseWithFile(const char filename[], html_variables data)
{
	File file = SPIFFS.open(filename);
	Serial.println(String("open file ") + filename);
	String content;
	content = file.readString();

	for(const auto& substitute : data)
	{
		content.replace(substitute.first, substitute.second);
	}

	webServer.send(200, "text/html", content);
	file.close();
}


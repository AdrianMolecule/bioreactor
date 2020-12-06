#include "httpserver.h"

#include <functional>
#include <Update.h>
#include <ArduinoJson.h>

#include "core/reactor.h"
#include "misc.h"

using namespace std::placeholders;

HTTPServer::HTTPServer(uint16_t HTTPPort, uint16_t webSocketPort)
: _ws_server(webSocketPort),
  _web_server(HTTPPort)
{

}

void HTTPServer::init(Reactor* reactor_mgr)
{
	_reactor_mgr = reactor_mgr;

	_web_server.on("/", std::bind(&HTTPServer::onMain, this));

	_web_server.on("/settings", std::bind(&HTTPServer::onSettings, this));

	_web_server.on("/firmware", HTTP_ANY,
			std::bind(
					&HTTPServer::responseWithFile,
					this,
					"/dummy.html",
					html_variables({{"##server_message##", "Reboot the board to apply changes"}})
			),
			std::bind(&HTTPServer::onFirmwareUpload, this)
	);

	_web_server.on("/program", std::bind(&HTTPServer::onProgram, this));

	_web_server.on("/upload", HTTP_ANY,
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

	_web_server.serveStatic("/header.html", SPIFFS, "/header.html");
	_web_server.serveStatic("/footer.html", SPIFFS, "/footer.html");
	_web_server.serveStatic("/dygraph.min.css", SPIFFS, "/dygraph.min.css");
	_web_server.serveStatic("/dygraph.min.js", SPIFFS, "/dygraph.min.js");

	_web_server.begin();
	_ws_server.begin();
	_ws_server.onEvent(std::bind(&HTTPServer::onWSEvent, this, _1, _2, _3, _4));
}

void HTTPServer::loop()
{
	_web_server.handleClient();
	_ws_server.loop();
}

void HTTPServer::sendWebSockData(String data)
{
	for(uint8_t client : _ws_connections)
	{
		_ws_server.sendTXT(client, data);
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
    	_ws_connections.erase(num);
      Serial.printf("[%u] Disconnected!\n", num);
      break;
    case WStype_CONNECTED:
      {
    	  _ws_connections.insert(num);
        IPAddress ip { _ws_server.remoteIP(num) };
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
	if(_web_server.method() == HTTPMethod::HTTP_POST)
	{
		Actuators* act_mgr = _reactor_mgr->get_actuators();

		if( _web_server.arg("fet1") == "on" )
			act_mgr->changeFET(0, true);
		else
			act_mgr->changeFET(0, false);

		if( _web_server.arg("fet2") == "on" )
			act_mgr->changeFET(1, true);
		else
			act_mgr->changeFET(1, false);

		act_mgr->changeHBridge(0, bridgeStateConvert(_web_server.arg("hbridge1")));
		act_mgr->changeHBridge(1, bridgeStateConvert(_web_server.arg("hbridge2")));
		act_mgr->changeHBridge(2, bridgeStateConvert(_web_server.arg("hbridge3")));
		act_mgr->changeHBridge(3, bridgeStateConvert(_web_server.arg("hbridge4")));

		if( _web_server.arg("led") == "on" )
			act_mgr->changeLED(true);
		else
			act_mgr->changeLED(false);

		if( _web_server.arg("motor") == "on" )
			act_mgr->changeMotor(true);
		else
			act_mgr->changeMotor(false);
	}

	Serial.println("connected " + _web_server.uri());
	responseWithFile("/index.html", {});
}

void HTTPServer::onSettings()
{
	if(_web_server.method() == HTTPMethod::HTTP_POST)
	{
		saveWifiSettings(_web_server.arg("ssid"), _web_server.arg("pass"));
	}

	String ssid, password;
	getWifiSettings(ssid, password);

	html_variables data;
	data["##ssid##"] = ssid;

	responseWithFile("/settings.html", data);
}

void HTTPServer::onProgram()
{
	const auto& programs = _reactor_mgr->read_programs_list();

	if(_web_server.method() == HTTPMethod::HTTP_POST)
	{
		Reactor::ProgramSettings settings{};

		if(_web_server.arg("new_program").isEmpty())
		{
			int id = _web_server.arg("id").toInt();
			if(id > programs.size())
			{
				id = _reactor_mgr->program_active();
			}
			settings = programs[static_cast<uint8_t>(id)];
		}

		if(!_web_server.arg("temp").isEmpty())
			settings.temp = _web_server.arg("temp").toFloat();
		if(!_web_server.arg("ph").isEmpty())
			settings.ph = _web_server.arg("ph").toFloat();
		if(!_web_server.arg("name").isEmpty())
			settings.name = _web_server.arg("name").c_str();

		Serial.printf("OnProgram: %d %s\n", _web_server.arg("new_program").isEmpty(), settings.name.c_str());

		_reactor_mgr->save_program(settings, _web_server.arg("enabled").toInt(), !_web_server.arg("new_program").isEmpty());
	}

	static StaticJsonDocument<300> programs_json;
	programs_json.clear();

	for(size_t i = 0; i < programs.size(); ++i)
	{
		programs_json["programs"][i];
		programs_json["programs"][i]["id"] = programs[i].id;
		programs_json["programs"][i]["name"] = programs[i].name.c_str();
		programs_json["programs"][i]["temp"] = programs[i].temp;
		programs_json["programs"][i]["ph"] = programs[i].ph;
	}

	programs_json["is_enabled"] = _reactor_mgr->program_enabled();
	programs_json["active"] = _reactor_mgr->program_active();

	String serialized_json;
	serializeJson(programs_json, serialized_json);
	html_variables data;
	data["##programs##"] = serialized_json;
	responseWithFile("/program.html", data);
}

void HTTPServer::onFirmwareUpload()
{
	HTTPUpload& upload = _web_server.upload();

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
  HTTPUpload& upload = _web_server.upload();

  if (upload.status == UPLOAD_FILE_START) {
    String filename = upload.filename;
    // Make sure paths always start with "/"
    if (!filename.startsWith("/")) {
      filename = "/" + filename;
    }
    _upload_file = SPIFFS.open(filename, "w");
    if (!_upload_file) {
    	Serial.println("CREATE FAILED");
      return;
    }

    Serial.println(String("Upload: START, filename: ") + filename);
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    if (_upload_file) {
      size_t bytesWritten = _upload_file.write(upload.buf, upload.currentSize);
      if (bytesWritten != upload.currentSize) {
        Serial.println("WRITE FAILED");
        return;
      }
    }
    Serial.println(String("Upload: WRITE, Bytes: ") + upload.currentSize);
  } else if (upload.status == UPLOAD_FILE_END) {
    if (_upload_file) {
      _upload_file.close();
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

	_web_server.send(200, "text/html", content);
	file.close();
}


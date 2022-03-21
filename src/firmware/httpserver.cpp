#include "httpserver.h"

#include <functional>
#include <Update.h>
#include <ArduinoJson.h>

#include "core/reactor.h"
#include "misc.h"

using namespace std::placeholders;

HTTPServer::HTTPServer(uint16_t HTTPPort, uint16_t webSocketPort)
: _ws_server(webSocketPort),
  _web_server(HTTPPort),
  _reactor_mgr(nullptr)
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

	_web_server.on("/program_history", std::bind(&HTTPServer::responseWithProgramFile, this));

	_web_server.on("/batchviewer", std::bind(&HTTPServer::onBatchView, this) );

	if(!SPIFFS.begin(true))
	{
		Serial.println("An Error has occurred while mounting SPIFFS");
		return;
	}

	_web_server.serveStatic("/header.html", SPIFFS, "/header.html");  // "max-age=86400"
	_web_server.serveStatic("/footer.html", SPIFFS, "/footer.html");
	_web_server.serveStatic("/dygraph.min.css", SPIFFS, "/dygraph.min.css", "max-age=86400");
	_web_server.serveStatic("/dygraph.min.js", SPIFFS, "/dygraph.min.js", "max-age=86400");

	_web_server.begin();
	_ws_server.begin();
	_ws_server.onEvent(std::bind(&HTTPServer::onWSEvent, this, _1, _2, _3, _4));
}

void HTTPServer::loop()
{
	_web_server.handleClient();
	_ws_server.loop();


	if(_reactor_mgr->_sensor_data.new_data_available)
	{
		String data = serializeState(_reactor_mgr, _reactor_mgr->_sensor_data.data.back());
		sendWebSockData(data);
		_reactor_mgr->_sensor_data.new_data_available = false;
	}
}

void HTTPServer::sendWebSockData(String data)
{
	for(uint8_t client : _ws_connections)
	{
		_ws_server.sendTXT(client, data);
	}
}

void HTTPServer::onWSEvent(uint8_t client_id,
                      WStype_t type,
                      uint8_t * payload,
                      size_t length)
{

  switch(type)
  {
    case WStype_DISCONNECTED:
    	_ws_connections.erase(client_id);
      Serial.printf("HTTPServer::onWSEvent: [%u] Disconnected!\n", client_id);
      break;
    case WStype_CONNECTED:
      {
    	_ws_connections.insert(client_id);
        IPAddress ip { _ws_server.remoteIP(client_id) };
        Serial.printf("HTTPServer::onWSEvent: [%u] Connection from ", client_id);
        Serial.println(ip.toString());

    	unsigned short sensor_rate_sec = -1;
    	getServerSettings(sensor_rate_sec);

    	time_t readings_time = _reactor_mgr->_sensor_data.start_time;
        for(const auto& it : _reactor_mgr->_sensor_data.data)
    	{
    		String data = serializeState(_reactor_mgr, it, readings_time);

    		_ws_server.sendTXT(client_id, data);
    		readings_time += sensor_rate_sec;
    	}

        Serial.printf("HTTPServer::onWSEvent: sensor data is sent, %u datagrams\n", _reactor_mgr->_sensor_data.data.size());

      }
      break;
    case WStype_TEXT:
	{
		Serial.printf("HTTPServer::onWSEvent: [%u] Text: %s\n", client_id, payload);

		static StaticJsonDocument<400> state;
		state.clear();
		DeserializationError error = deserializeJson(state, payload);

		if (error)
		{
			Serial.printf("deserializeJson() failed: %s\n", error.c_str());
		    break;
		}

		Actuators* act_mgr = _reactor_mgr->get_actuators();

		for(size_t i = 0; i < state["fet"].size(); ++i)
			act_mgr->changeFET(i, state["fet"][i]);

		for(size_t i = 0; i < state["hbridge"].size(); ++i)
			act_mgr->changeHBridge(i, bridgeStateConvert(static_cast<const char*>(state["hbridge"][i][0])), state["hbridge"][i][1] );

		act_mgr->changeLED(state["led"]);
		act_mgr->changeMotor(state["motor"]);

		break;
	}
    default:
      break;
  }
}

void HTTPServer::onMain()
{
	Serial.println("connected " + _web_server.uri());
	responseWithFile("/index.html", {});
}

void HTTPServer::onSettings()
{
	if(_web_server.method() == HTTPMethod::HTTP_POST)
	{
		saveWifiSettings(_web_server.arg("ssid"), _web_server.arg("pass"));

		if(!_web_server.arg("sensor_rate").isEmpty())
		{
			unsigned short sensor_rate = std::max<unsigned short>(_web_server.arg("sensor_rate").toInt(), 1);
			saveServerSettings(sensor_rate);
			_reactor_mgr->schedule_routines(sensor_rate);
		}
	}

	String ssid, password;
	getWifiSettings(ssid, password);

	unsigned short sensor_read_rate = -1;
	getServerSettings(sensor_read_rate);

	html_variables data;
	data["##ssid##"] = ssid;
	data["##sensor_rate##"] = sensor_read_rate;

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

void HTTPServer::responseWithProgramFile()
{
	File file = SPIFFS.open(String("/") + _web_server.arg("file"));

	if(!file)
	{
		Serial.println("HTTPServer::responseWithProgramFile: file doesn't exist");
		return;
	}
	Serial.println("HTTPServer::responseWithProgramFile: send data");
	SensorState::Readings sensor_data;
	constexpr size_t struct_length = sizeof(SensorState::Readings);

	_web_server.setContentLength(CONTENT_LENGTH_UNKNOWN);
	_web_server.send(200, "text/plain", "");

	String data;
	_web_server.sendContent("[");
	while( file.available() )
	{
		file.read((uint8_t*)&sensor_data, struct_length);

    	static StaticJsonDocument<200> sensor_json;
    	sensor_json.clear();

    	JsonObject object = sensor_json.to<JsonObject>();

    	sensor_data.serializeState(object);

		serializeJson(sensor_json, data);

		_web_server.sendContent(data);
		data = ",";		// append a comma separator for the next cycle iteration
	}

    _web_server.sendContent("]");
}

void HTTPServer::onBatchView()
{
	html_variables data;

	if(_web_server.arg("current").toInt() == 1)
		data["##filename##"] = "currentrun";
	else
		data["##filename##"] = "lastrun";

	responseWithFile("/viewer.html", data);
}

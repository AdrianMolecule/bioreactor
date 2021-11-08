#ifndef HTTPServer_H
#define HTTPServer_H

#include <stdint.h>

#include <WebServer.h>
#include <WebSocketsServer.h>
#include <unordered_set>
#include <unordered_map>
#include "SPIFFS.h"

class Reactor;

class HTTPServer
{
public:
	HTTPServer(uint16_t HTTPPort = 80, uint16_t webSocketPort = 81);

	void init(Reactor* reactor_mgr);
	void loop();
	void sendWebSockData(String data);

private:
	using html_variables = std::unordered_map<const char*, String>;
	void onWSEvent(uint8_t client_id, WStype_t type, uint8_t * payload, size_t length);
	void onMain();
	void onSettings();
	void onProgram();
	void onFile();
	void onFirmwareUpload();
	void handleFileUpload();
	void responseWithFile(const char filename[], html_variables data);

	WebSocketsServer _ws_server;
	WebServer _web_server;
	std::unordered_set<uint8_t> _ws_connections;
	Reactor* _reactor_mgr;
	File _upload_file;
};

#endif

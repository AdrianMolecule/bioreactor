#ifndef HTTPServer_H
#define HTTPServer_H

#include <stdint.h>

#include <WebServer.h>
#include <WebSocketsServer.h>
#include <unordered_set>

class ReactorState;

class HTTPServer
{
public:
	HTTPServer(uint16_t HTTPPort = 80, uint16_t webSocketPort = 81);

	void init(std::shared_ptr<ReactorState> reactor);
	void loop();
	void sendWebSockData(String data);

private:
	void onWSEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length);
	void onHTTPConnect();
	void onSettings();
	void onProgram();
	void onFile();

	WebSocketsServer WSServer;
	WebServer webServer;
	std::unordered_set<uint8_t> WSConnections;
	std::shared_ptr<ReactorState> reactorState;
};

#endif

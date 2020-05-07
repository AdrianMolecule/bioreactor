#include "hwinit.h"

#include <WiFi.h>
#include "config.h"
#include "lwip/inet.h"

void WiFiConnect(bool isAccessPoint)
{
	if(isAccessPoint)
	{
		WiFi.softAP(config::AP_ssid);
		IPAddress t(192,168,0,1);
		IPAddress t1(255,255,255,1);
		WiFi.softAPConfig(t,t,t1);
	  	/*WiFi.softAPConfig(
	  			inet_addr(config::AP_ip),
				inet_addr(config::AP_gateway),
				inet_addr(config::AP_subnet)
	  	);
	  	*/
	}
	else
	{
		while (WiFi.status() != WL_CONNECTED)
		{
			WiFi.begin(config::ssid, config::password);
			delay(1000);
			//display->print("Connecting to WiFi..");
		}
	}

}

#ifndef CONFIG_H
#define CONFIG_H

namespace config
{
	constexpr char domainName[] = "bioreactor";	// real domain will be with label ".local", ex "http://bioreactor.local"

	//---------- WiFi network
	constexpr char ssid[] = "natashka";
	constexpr char password[] = "12345678";

	//--------- WiFi access point
	constexpr char AP_ssid[] = "bioreactor";
	constexpr char AP_ip[] = "192.168.1.1";
	constexpr char AP_gateway[] = "192.168.1.1";
	constexpr char AP_subnet[] = "255.255.255.1";
};

#endif

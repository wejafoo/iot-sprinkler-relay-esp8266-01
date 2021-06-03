

#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

#ifndef STASSID
	#define	STASSID	"bilbo"
	#define	STAPSK	"readyplayer1"
#endif

#ifdef USE_LittleFS							// Silences spiff deprecation warnings, better way?
	#define		SPIFFS LittleFS
	#include	<LittleFS.h>
#endif
// #include	<FS.h>

#define API_TEST	false					// Deployment-specific settings. Review before flashing new board.
#define API_LOCAL	true
#define HOST_LOCAL	"dev.weja.us"
#define HOST_TEST	"too.fb.weja.us"
#define HOST_PROD	"foo.fb.weja.us"
#define PORT_REMOTE	443
#define PORT_LOCAL	5423
#define PODID		1
#define SLEEP		3600000					// default: 3600000 -- 1 hour
#define SLEEPDOH	1000					// default: 30000 -- .5 minutes
#define ZONE1		3						// connect to GPIO3
#define ZONE2		2						// connect to GPIO2
#define ZONE3		1						// connect to GPIO1
#define ZONE4		0						// connect to GPIO0

const 	int			zone1		= ZONE1;
const 	int			zone2		= ZONE2;
		int			zone3		= ZONE3;
const 	int			zone4		= ZONE4;
const	bool		isDebug		= false;
const	bool		apiIsLocal	= API_LOCAL;
const	bool		apiIsTest	= API_TEST;
const	char *		ssid		= STASSID;
const	char *		pwd			= STAPSK;
const	int			podId		= PODID;
const	int			sleep		= SLEEP;
const	int			sleepOnDoh	= SLEEPDOH;
		String		host		= HOST_LOCAL;
		int			port		= PORT_LOCAL;
		String		deviceMacId	= "";
		WiFiClient	client;

/*
	Todo: Add builtin button reference using 'INPUT_PULLUP' on running unit to claw back TX serial log
			for troubleshooting quad relays, of course, sacrificing gpio0 for the privilege.
*/

void bootTest() {
	delay(5000);
	if (isDebug) Serial.println(WiFi.macAddress());
	digitalWrite(zone1, LOW);
	delay(5000);
	if (isDebug) Serial.println(WiFi.macAddress());
	digitalWrite(zone1, HIGH);
	delay(5000);
	if (isDebug) Serial.println(WiFi.macAddress());
	digitalWrite(zone2, LOW);
	delay(5000);
	if (isDebug) Serial.println(WiFi.macAddress());
	digitalWrite(zone2, HIGH);
	delay(5000);
	if (isDebug) Serial.println(WiFi.macAddress());
	digitalWrite(zone3, LOW);
	delay(5000);
	if (isDebug) Serial.println(WiFi.macAddress());
	digitalWrite(zone3, HIGH);
	delay(5000);
	if (isDebug) Serial.println(WiFi.macAddress());
	digitalWrite(zone4, LOW);
	delay(5000);
	if (isDebug) Serial.println(WiFi.macAddress());
	digitalWrite(zone4, HIGH);
}

void setup() {
	if (isDebug) {
		pinMode(zone1, FUNCTION_3);	// Now playing the part of TX, will be gpio3. Recoup: pinMode(1, FUNCTION_0). Use only if serial.begin has already commandeered the TX pin.
		pinMode(zone1, OUTPUT);
		Serial.begin(115200);
		zone3 = 2;
	} else {
		pinMode(zone1, FUNCTION_3);
		pinMode(zone3, FUNCTION_3);
		pinMode(zone1, OUTPUT);
		pinMode(zone3, OUTPUT);
	}
	pinMode(zone2, OUTPUT);
	pinMode(zone4, OUTPUT);
	digitalWrite(zone1, HIGH);				// Init all zones HIGH=OFF(referring to sprinkler solenoid)
	digitalWrite(zone2, HIGH);
	digitalWrite(zone3, HIGH);
	digitalWrite(zone4, HIGH);
	WiFi.mode(WIFI_STA);					// Limit access to ONLY WiFi connects(i.e.internal router disabled)
	deviceMacId	= WiFi.macAddress();		// Use ONLY to confirm device config, NOT in lieu of actual security!
	WiFi.begin(ssid, pwd);
	while ( WiFi.status() != WL_CONNECTED ) {
		if (isDebug) Serial.print(".");
		delay(1000);
	}
	bootTest();
}

void loop() {
	if (apiIsLocal) {
		host = HOST_LOCAL;
	} else if (apiIsTest) {
		host = HOST_TEST;
		port = PORT_REMOTE;
		WiFiClientSecure client2;		// Todo: Make WiFiClient conditionally inherit WifiClientSecure for non-local
		client2.setInsecure();			// Skip surprisingly tedious verification for now |:^o Todo: consider SSL fingerprint impl
	} else {
		host = HOST_PROD;
		port = PORT_REMOTE;
		WiFiClientSecure client2;		// Todo: Make WiFiClient conditionally inherit WifiClientSecure for non-local
		client2.setInsecure();			// Skip surprisingly tedious verification for now |:^o Todo: consider SSL fingerprint impl
	}

	client.setTimeout(20000);
	if ( client.connect(host, port) ) {
		if (isDebug) Serial.println("connected... continuing");
	} else {
		if (isDebug) Serial.println("not connected... retrying");
		delay(sleepOnDoh);
		return;
	}
	client.printf("GET /api/pod/%d?device=%s HTTP/1.1", podId, deviceMacId.c_str());
	client.printf("\r\nHost: %s\r\nReferer: %s\r\nAccept: application/json\r\nConnection: close\r\n\r\n", host.c_str(), deviceMacId.c_str());

	while ( client.connected() || client.available() ) {
		if ( client.available() ) {
			String line = client.readStringUntil('\n');
			if (line == "\r") break;
		}
	}

	DynamicJsonDocument doc(2560);				// FYI, arduinojson.org/v6/assistant way underestimated with 256
	DeserializationError error = deserializeJson(doc, client);
	if ( error ) {
		if (isDebug) Serial.printf("????????????????????? %s", error.c_str());
		client.stop();
		delay(sleepOnDoh);		// Override the full loop cycle and try to connect again sooner... not too soon though
		return;
	}

	const JsonObject	podObj	= doc.as<JsonObject>();
	const int			pod		= podObj["pod"];
	if ( pod == podId ) {
		JsonObject zoneObj	= podObj["payload"].as<JsonObject>();
		if ( zoneObj.containsKey("1")||zoneObj.containsKey("2")||zoneObj.containsKey("3")||zoneObj.containsKey("4")) {
			if (zoneObj.containsKey("1")) {
				int duration = zoneObj["1"].as<int>();
				if (duration > 0) {
					digitalWrite(zone1, LOW);
					delay(duration);
					digitalWrite(zone1, HIGH);
				}
			}
			if (zoneObj.containsKey("2")) {
				int duration = zoneObj["2"].as<int>();
				if (duration > 0) {
					digitalWrite(zone2, LOW);
					delay(duration);
					digitalWrite(zone2, HIGH);
				}
			}
			if (zoneObj.containsKey("3")) {
				int duration = zoneObj["3"].as<int>();
				if (duration > 0) {
					digitalWrite(zone3, LOW);
					delay(duration);
					digitalWrite(zone3, HIGH);
				}
			}
			if (zoneObj.containsKey("4")) {
				int duration = zoneObj["4"].as<int>();
				if (duration > 0) {
					digitalWrite(zone4, LOW);
					delay(duration);
					digitalWrite(zone4, HIGH);
				}
			}
		} else {
			if (isDebug) Serial.println("no instructions in pod1 object\n...  ZzZzZzZzZzZzZzZzZzZz ...");
		}
	} else {
		if (isDebug) Serial.println("nothing doing\n...  ZzZzZzZzZzZzZzZzZzZz  ...");
	}
	client.stop();
	if (isDebug) Serial.println("...  ZzZzZzZzZzZzZzZzZzZz  ...");
	delay(sleep);
}

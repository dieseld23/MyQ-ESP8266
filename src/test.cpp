#include <Arduino.h>
#include <ArduinoJson.h>
#include <ArduinoOTA.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266HTTPUpdateServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>  // Useful to access to ESP by hostname.local
#include <LittleFS.h>
#include <Ticker.h>	 // For LED status
#include <WiFiClientSecureBearSSL.h>
#include <WiFiManager.h>  // https://github.com/tzapu/WiFiManager WiFi Configuration Magic
#include <WiFiUdp.h>
#include <myq.h>

/// ##### Start user configuration ######
const int configpin = 0;
int port = 80;
const int ledpin = LED_BUILTIN;
String myqUsername;
String myqPassword;
/// ##### End user configuration ######

const bool enableMDNSServices = true;
char wifi_config_name[32] = "ESP Setup";	// Default
char host_name[20] = "DM";				// Default
bool shouldSaveConfig = false;			// Flag for saving data
File fsUploadFile;
Ticker led1tick;
ESP8266WebServer server(80);
ESP8266HTTPUpdateServer httpUpdateServer;
MyQ myq("", myqUsername, myqPassword, "");
MyQ_devices_t *MyQ_devices[10];				// up to 10 devices
MyQ_account_t *MyQ_account;

//+=============================================================================
// convert the file extension to the MIME type
//
String getContentType(String filename) {
	if (filename.endsWith(".html"))
		return "text/html";
	else if (filename.endsWith(".css"))
		return "text/css";
	else if (filename.endsWith(".js"))
		return "application/javascript";
	else if (filename.endsWith(".ico"))
		return "image/x-icon";
	else if (filename.endsWith(".gz"))
		return "application/x-gzip";
	return "text/plain";
}

//+=============================================================================
//  send the right file to the client (if it exists)
//
bool handleFileRead(String path) {
	//  Serial.println("handleFileRead: " + path);
	if (path.endsWith("/"))
		path += "index.html";
	// If a folder is requested, send the index file
	String contentType = getContentType(path);
	// Get the MIME type
	String pathWithGz = path + ".gz";
	if (LittleFS.exists(pathWithGz) || LittleFS.exists(path)) {
		// If the file exists, either as a compressed archive, or normal
		if (LittleFS.exists(pathWithGz))  // If there's a compressed version available
			path += ".gz";				  // Use the compressed verion
		File file = LittleFS.open(path, "r");
		//  Open the file
		server.streamFile(file, contentType);
		//  Send it to the client
		file.close();
		// Close the file again
		// Serial.println(String("\tSent file: ") + path);
		return true;
	}
	// Serial.println(String("\tFile Not Found: ") + path);
	// If the file doesn't exist, return false
	return false;
}
//+=============================================================================
// Upload a new file to the LittleFS
//
void handleFileUpload() {
	HTTPUpload &upload = server.upload();
	if (upload.status == UPLOAD_FILE_START) {
		String filename = upload.filename;
		if (!filename.startsWith("/")) {
			filename = "/" + filename;
		}
		// Serial.print("handleFileUpload Name: "); //Serial.println(filename);
		fsUploadFile = LittleFS.open(filename, "w");
		// Open the file for writing in LittleFS (create if it doesn't exist)
		filename = String();
	} else if (upload.status == UPLOAD_FILE_WRITE) {
		if (fsUploadFile) {
			fsUploadFile.write(upload.buf, upload.currentSize);
			// Write the received bytes to the file
		}
	} else if (upload.status == UPLOAD_FILE_END) {
		if (fsUploadFile) {
			// If the file was successfully created
			fsUploadFile.close();
			// Close the file again
			// Serial.print("handleFileUpload Size: ");
			// Serial.println(upload.totalSize);
			server.sendHeader("Location", "/success.html");
			// Redirect the client to the success page
			server.send(303);
		} else {
			server.send(500, "text/plain", "500: couldn't create file");
		}
	}
}

//+=============================================================================
// Upload a new file to the LittleFS - File not found
//
void handleNotFound() {
	String message = "File Not Found\n\n";
	message += "URI: ";
	message += server.uri();
	message += "\nMethod: ";
	message += (server.method() == HTTP_GET) ? "GET" : "POST";
	message += "\nArguments: ";
	message += server.args();
	message += "\n";
	for (uint8_t i = 0; i < server.args(); i++) {
		message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
	}
	server.send(404, "text/plain", message);
}

//+=============================================================================
// Toggle state
//
void led1Ticker() {
	int state = digitalRead(ledpin);  // get the current state of GPIO1 pin
	digitalWrite(ledpin, !state);	  // set pin to the opposite state
}

//+=============================================================================
// Turn off the Led after timeout
//
void led1TickerDisable() {
	// Serial.println("Turning off the LED to save power.");
	digitalWrite(ledpin, HIGH);	 // Shut down the LED
	led1tick.detach();			 // Stopping the ticker
}

//+=============================================================================
// Callback notifying us of the need to save config
//
void saveConfigCallback() {
	Serial.println("Should save config");
	shouldSaveConfig = true;
}

//+=============================================================================
// Gets called when WiFiManager enters configuration mode
//
void configModeCallback(WiFiManager *myWiFiManager) {
	Serial.println("Entered config mode");
	Serial.println(WiFi.softAPIP());
	// if you used auto generated SSID, print it
	Serial.println(myWiFiManager->getConfigPortalSSID());
	// entered config mode, make led toggle faster
	led1tick.attach(0.2, led1Ticker);
}

//+=============================================================================
// Gets called when device loses connection to the accesspoint
//
void lostWifiCallback(const WiFiEventStationModeDisconnected &evt) {
	Serial.println("Lost Wifi");
	// reset and try again, or maybe put it to deep sleep
	ESP.reset();
	delay(1000);
}

//+=============================================================================
// Enable MDNS Function
//
void enableMDNS() {
	// Configure OTA Update
	ArduinoOTA.setPort(8266);
	ArduinoOTA.setHostname(host_name);
	ArduinoOTA.onStart([]() { Serial.println("Start"); });
	ArduinoOTA.onEnd([]() { Serial.println("\nEnd"); });
	ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) { Serial.printf("Progress: %u%%\r", (progress / (total / 100))); });
	ArduinoOTA.onError([](ota_error_t error) {
		Serial.printf("Error[%u]: ", error);
		if (error == OTA_AUTH_ERROR)
			Serial.println("Auth Failed");
		else if (error == OTA_BEGIN_ERROR)
			Serial.println("Begin Failed");
		else if (error == OTA_CONNECT_ERROR)
			Serial.println("Connect Failed");
		else if (error == OTA_RECEIVE_ERROR)
			Serial.println("Receive Failed");
		else if (error == OTA_END_ERROR)
			Serial.println("End Failed");
	});
	ArduinoOTA.begin();
	Serial.println("ArduinoOTA started");

	// Configure mDNS
	MDNS.addService("http", "tcp", port);  // Announce the ESP as an HTTP service
	Serial.println("MDNS http service added. Hostname is set to " + String(host_name) + ".local:" + String(port));
}

//+=============================================================================
// Setup HTTP server
//
void serverSetup() {
	server.on(
		"/file-upload", HTTP_POST,
		[]() {				   // if the client posts to the upload page
			server.send(200);  // Send status 200 (OK) to tell the client we are ready to receive
		},
		handleFileUpload);	// Receive and save the file

	server.on("/file-upload", HTTP_GET, []() {	// if the client requests the upload page
		String html = "<form method=\"post\" enctype=\"multipart/form-data\">";
		html += "<input type=\"file\" name=\"name\">";
		html += "<input class=\"button\" type=\"submit\" value=\"Upload\">";
		html += "</form>";
		server.send(200, "text/html", html);
	});

	server.on("/", []() {
		server.sendHeader("Location", String("ui.html"), true);
		server.send(302, "text/plain", "");
	});

	server.on("/reset", []() {
		server.send(200, "text/html", "reset");
		delay(100);
		ESP.restart();
	});

	server.serveStatic("/", LittleFS, "/", "max-age=86400");

	server.onNotFound(handleNotFound);
}

//+=============================================================================
// Setup Wifi
//
bool setupWifi(bool resetConf) {
	led1tick.attach(0.5, led1Ticker);  // start ticker with 0.5 because we start in AP mode and try to connect
	// WiFiManager Local intialization. Once its business is done, there is no need to keep it around
	WiFiManager wifiManager;
	if (resetConf)	// reset settings - for testing
		wifiManager.resetSettings();

	wifiManager.setAPCallback(configModeCallback);	// set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
	wifiManager.setSaveConfigCallback(saveConfigCallback);	// set config save notify callback
	wifiManager.setConfigPortalTimeout(180);				// Reset device if on config portal for greater than 3 minutes

	if (LittleFS.begin()) {
		Serial.println("Mounted file system");
		if (LittleFS.exists("/config.json")) {	// file exists, reading and loading

			Serial.println("Reading config file");
			File configFile = LittleFS.open("/config.json", "r");
			if (configFile) {
				Serial.println("Opened config file");
				size_t size = configFile.size();  // Allocate a buffer to store contents of the file.
				std::unique_ptr<char[]> buf(new char[size]);
				configFile.readBytes(buf.get(), size);
				DynamicJsonDocument json(1024);
				DeserializationError error = deserializeJson(json, buf.get());
				serializeJson(json, Serial);
				if (!error) {
					Serial.println("\nParsed json");
					if (json.containsKey("hostname")) {
						strncpy(host_name, json["hostname"], 20);
					}
					if (json.containsKey("myqusername")) {
						myqUsername = json["myqusername"].as<String>();
					}
					if (json.containsKey("myqpassword")) {
						myqPassword = json["myqpassword"].as<String>();
					}
				} else {
					Serial.println("Failed to load json config");
				}
				configFile.close();
			}
		}
	} else {
		Serial.println("Failed to mount FS");
	}

	WiFiManagerParameter custom_hostname("hostname", "Choose a hostname to this IR Controller", host_name, 20);
	WiFiManagerParameter custom_myqUsername("Login ID", "MyQ User Login", myqUsername.c_str(), 40);
	WiFiManagerParameter custom_myqPassword("Password", "MyQ Password", myqPassword.c_str(), 40);

	wifiManager.addParameter(&custom_hostname);
	wifiManager.addParameter(&custom_myqUsername);
	wifiManager.addParameter(&custom_myqPassword);
	
	// fetches ssid and pass and tries to connect, if it does not connect it starts an access point with the specified name and goes into a blocking loop
	// awaiting configuration
	if (!wifiManager.autoConnect(wifi_config_name)) {
		Serial.println("Failed to connect and hit timeout");  // reset and try again, or maybe put it to deep sleep
		delay(1000);
		ESP.reset();
		delay(2000);
	}

	// if you get here you have connected to the WiFi
	strncpy(host_name, custom_hostname.getValue(), 20);
	myqUsername = String(custom_myqUsername.getValue());
	myqPassword = String(custom_myqPassword.getValue());

	WiFi.onStationModeDisconnected(&lostWifiCallback);	// Reset device if lost wifi Connection
	Serial.println("WiFi connected! User chose hostname '" + String(host_name) + "'");
	if (shouldSaveConfig) {	 // save the custom parameters to FS
		Serial.println(" Config...");
		DynamicJsonDocument json(100);
		json["hostname"] = host_name;
		json["myqusername"] = myqUsername;
		json["myqpassword"] = myqPassword;

		File configFile = LittleFS.open("/config.json", "w");
		if (!configFile) {
			Serial.println("Failed to open config file for writing");
		}
		serializeJson(json, Serial);
		Serial.println("");
		Serial.println("Writing config file");
		serializeJson(json, configFile);
		configFile.close();
		json.clear();
		Serial.println("Config written successfully");
	}
	led1tick.detach();
	digitalWrite(ledpin, LOW);	// keep LED on
	return true;
}

//+=============================================================================
// Main Setup
//
void setup() {
	pinMode(ledpin, OUTPUT);
	pinMode(configpin, INPUT_PULLUP);
	Serial.begin(115200);
	delay(5000);

	Serial.println("");
	Serial.println("ESP8266 IR Controller");
	pinMode(configpin, INPUT_PULLUP);
	Serial.print("Config pin GPIO");
	Serial.print(configpin);
	Serial.print(" set to: ");
	Serial.println(digitalRead(configpin));
	if (!setupWifi(digitalRead(configpin) == LOW))
		return;
	Serial.println("WiFi configuration complete");
	if (strlen(host_name) > 0) {
		WiFi.hostname(host_name);
	} else {
		WiFi.hostname().toCharArray(host_name, 20);
	}
	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		Serial.print(".");
	}
	wifi_set_sleep_type(LIGHT_SLEEP_T);
	digitalWrite(ledpin, LOW);

	led1tick.attach(2, led1TickerDisable);	// Turn off the led in 2s
	Serial.print("Local IP: ");
	Serial.println(WiFi.localIP().toString());
	Serial.println("URL to send commands: http://" + String(host_name) + ":" + port);

	if (enableMDNSServices) {
		enableMDNS();
	}
	myq.setLogin(myqUsername, myqPassword);	
	httpUpdateServer.setup(&server);
	serverSetup();
	server.begin();
	
}

void loop() {
	ArduinoOTA.handle();
	server.handleClient();
	MDNS.update();
	
	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		Serial.print(".");
  	}
	  Serial.println(myqUsername);
	myq.login();
	myq.getDevices();
	delay(30000);


}
#ifndef MYQ_H_
#define MYQ_H_

#include <Arduino.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <JSON_Decoder.h>
#include <JSON_Listener.h>
#include <WiFiClientSecureBearSSL.h>

/*
static const struct errorMessages_t {
	const String e11 = "Something unexpected happened. Please wait a bit and try again.";
	const String e12 = "MyQ service is currently down. Please wait a bit and try again.";
	const String e13 = "Not logged in.";
	const String e14 = "Email and/or password are incorrect.";
	const String e15 = "Invalid parameter(s) provided.";
	const String e16 = "User will be locked out due to too many tries. 1 try left.";
	const String e17 = "User is locked out due to too many tries. Please reset password and try again.";
	const String e18 = "The requested device could not be found.";
	const String e19 = "Unable to determine the state of the requested device.";
	const String e20 = "Could not find that URL. Please file a bug report.";
} errorMessages;
*/

struct routes_t {
	const String account = "/My";
	String getDevices = "/Accounts/{accountId}/Devices";
	const String login = "/Login";
	String setDevice = "/Accounts/{accountId}/Devices/{serialNumber}/actions";
	String users = "/Accounts/{accountId}/Users";
	String accessGroups = "/Accounts/{accountId}/Users";
	String roles = "/Accounts/{accountId}/Roles";
	String accessSchedules = "/Accounts/{accountId}/AccessSchedules";
	String zones = "/Accounts/{accountId}/Zones";
};


class MyQ {
   public:
	MyQ(String accountId, String username, String password, String securityToken);
	void setLogin(String username, String password);
	void login();
	bool checkIsLoggedIn();
	bool getAccountInfo();
	void getDevices();
	const char* getDeviceState(const char *serial_number, const char *attributeName);
	void getDoorState();
	void getLightState();
	void setDeviceState();
	void setDoorOpen();
	void setLightState();

	void getData(String route, String method, String data = "");
void printHeapFreeToSerial();

   protected:
	String _accountId;
	String _username;
	String _password;
	String _securityToken;

   private:
};


#endif /* MYQ_H_ */
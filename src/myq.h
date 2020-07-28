#ifndef MYQ_H_
#define MYQ_H_

#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecureBearSSL.h>

const struct allDeviceTypes_t {
	const String hub = "hub";
	const String virtualGarageDoorOpener = "virtualgaragedooropener";
	const String wifiGarageDoorOpener = "wifigaragedooropener";
	const String wifiGdoGateway = "wifigdogateway";
} allDeviceTypes;

const struct errorMessages_t {
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

const struct doorCommands_t {
	const String close = "close";
	const String open = "open";
} doorCommands;

const struct doorStates_t {
	const String s1 = "open";
	const String s2 = "closed";
	const String s3 = "stopped in the middle";
	const String s4 = "going up";
	const String s5 = "going down";
	const String s9 = "not closed";
} doorStates;

const struct lightCommands_t {
	const String on = "on";
	const String off = "off";
} lightCommands;

const struct lightStates_t {
	const String s0 = "turnoff";
	const String s1 = "turnon";
} lightStates;

const struct myQProperties_t {
	const String doorState = "door_state";
	const String lastUpdate = "last_update";
	const String lightState = "light_state";
	const String online = "online";
} myQProperties;

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

// const struct result_t {
//	int returnCode;
//   String message;
//    String providerMessage;
//   String unhandledError;
//} result;

// struct headers: {
//  defaultUserAgent: "okhttp/3.10.0",
//  deviceApiVersion: deviceVersion,
//  defaultBrandId: 2,
//  defaultCulture: "en",
//  appId: "JVM/G9Nwih5BwKgNCjLxiFUQxQijAebyyg8QUHr7JOrP+tuPb8iHfRHKwTmDzHOu",
// },
/*
struct MyQ_account_t {
	const char* Account_Id;
	const char* Account_Name;
	const char* Account_Email;
	const char* Account_Address_AddressLine1;
	const char* Account_Address_AddressLine2;
	const char* Account_Address_City;
	const char* Account_Address_State;
	const char* Account_Address_PostalCode;
	const char* Account_Address_Country_Code;
	bool Account_Address_Country_IsEEACountry;
	const char* Account_Phone;
	const char* Account_ContactName;
	int Account_DirectoryCodeLength;
	int Account_UserAllowance;
	const char* Account_TimeZone;
	const char* AnalyticsId;
	const char* UserId;
	const char* UserName;
	const char* Email;
	const char* FirstName;
	const char* LastName;
	const char* CultureCode;
	const char* Address_PostalCode;
	const char* Address_Country_Code;
	bool Address_Country_IsEEACountry;
	const char* TimeZone_Id;
	const char* TimeZone_Name;
	bool MailingListOptIn;
	bool RequestAccountLinkInfo;
	const char* Phone;
	bool DiagnosticDataOptIn;
};

struct MyQ_devices_t {
    const char* serial_number;
    const char* device_family;
    const char* device_platform;
    const char* device_type;
    const char* name;
    const char* created_date;
    const char* state_firmware_version;
    bool state_homekit_capable;
    bool state_homekit_enabled;
    const char* state_learn;
    bool state_learn_mode;
    const char* state_updated_date;
    const char* state_physical_devices[10];
    bool state_pending_bootload_abandoned;
    bool state_online; 
    const char* state_last_status;
    const char* parent_device;
    const char* parent_device_id;
    bool state_gdo_lock_connected;
    bool state_attached_work_light_error_present;
    const char* state_door_state;
    const char* state_open;
    const char* state_close;
    const char* state_last_update;
    const char* state_passthrough_interval;
    const char* state_door_ajar_interval;
    const char* state_invalid_credential_window;
    const char* state_invalid_shutout_period;
    bool state_is_unattended_open_allowed;
    bool state_is_unattended_close_allowed;
    const char* state_aux_relay_delay;
    bool state_use_aux_relay;
    const char* state_aux_relay_behavior;
    bool state_rex_fires_door;
    bool state_command_channel_report_status;
    bool state_control_from_browser;
    bool state_report_forced;
    bool state_report_ajar;
    int state_max_invalid_attempts;
    };
*/
class MyQ {
   public:
	MyQ(String accountId, String username, String password, String securityToken);
	void login();
	bool checkIsLoggedIn();
	DynamicJsonDocument executeRequest(String route, String method, String data);
	String getAccountInfo();
	void getDevices();
	void getDeviceState();
	void getDoorState();
	void getLightState();
	void setDeviceState();
	void setDoorOpen();
	void setLightState();
	//void parseData(DynamicJsonDocument doc, String route);

   protected:
	String _accountId;
	String _username;
	String _password;
	String _securityToken;

   private:
};

class errorHandler {
   public:
	// result_t parseBadResponse(int response);
	// result_t returnError(int returnCode, String error, String response);
   protected:
   private:
};

#endif /* MYQ_H_ */
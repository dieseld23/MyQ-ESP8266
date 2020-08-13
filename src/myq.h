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

// Structures  to hold the parsed values
struct MyQ_account_t {
	String Account_Id;
	// const char* Account_Name;
	// const char* Account_Email;
	// const char* Account_Address_AddressLine1;
	// const char* Account_Address_AddressLine2;
	// const char* Account_Address_City;
	// const char* Account_Address_State;
	// const char* Account_Address_PostalCode;
	// const char* Account_Address_Country_Code;
	// bool Account_Address_Country_IsEEACountry;
	// const char* Account_Phone;
	// const char* Account_ContactName;
	// int Account_DirectoryCodeLength;
	// int Account_UserAllowance;
	// const char* Account_TimeZone;
	// const char* AnalyticsId;
	String UserId;
	// const char* UserName;
	// const char* Email;
	// const char* FirstName;
	// const char* LastName;
	// const char* CultureCode;
	// const char* Address_PostalCode;
	// const char* Address_Country_Code;
	// bool Address_Country_IsEEACountry;
	// const char* TimeZone_Id;
	// const char* TimeZone_Name;
	// bool MailingListOptIn;
	// bool RequestAccountLinkInfo;
	// const char* Phone;
	// bool DiagnosticDataOptIn;
};

struct MyQ_devices_t {
	String serial_number;
	String device_family;
	String device_platform;
	String device_type;
	String name;
	// String created_date;
	// String state_firmware_version;
	// bool state_homekit_capable;
	// bool state_homekit_enabled;
	// const char* state_learn;
	// bool state_learn_mode;
	String state_updated_date;
	// const char* state_physical_devices[10];
	// bool state_pending_bootload_abandoned;
	bool state_online;
	String state_last_status;
	// const char* parent_device;
	// const char* parent_device_id;
	// bool state_gdo_lock_connected;
	// bool state_attached_work_light_error_present;
	String state_door_state;
	// const char* state_open;
	// const char* state_close;
	String state_last_update;
	// const char* state_passthrough_interval;
	// const char* state_door_ajar_interval;
	// const char* state_invalid_credential_window;
	// const char* state_invalid_shutout_period;
	// bool state_is_unattended_open_allowed;
	// bool state_is_unattended_close_allowed;
	// const char* state_aux_relay_delay;
	// bool state_use_aux_relay;
	// const char* state_aux_relay_behavior;
	// bool state_rex_fires_door;
	// bool state_command_channel_report_status;
	// bool state_control_from_browser;
	// bool state_report_forced;
	// bool state_report_ajar;
	// int state_max_invalid_attempts;
};

class MyQ : public JsonListener {
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
	void error(const char *message);
	void key(const char *key);
	void value(const char *value);
	void startDocument();
	void endDocument();
	void startObject();
	void endObject();
	void startArray();
	void endArray();

   protected:
	String _accountId;
	String _username;
	String _password;
	String _securityToken;

   private:
	bool ended = true;			 // Flag to indicate document has ended
	uint8_t _count = 0;
	String currentParent;		 // Current object e.g. "request"
	String currentKey;			 // Name key of the name:value pair e.g "temperature"
	uint16_t arrayIndex;		 // Array index 0-N e.g. 4 for 5th pass, qualify with valuePath
	String valuePath;			 // object (i.e. sequential key) path (like a "file path")
								 // taken to the name:value pair in the form "/response"
								 // so values can be pulled from the correct array.
								 // Needed since different objects contain "data" arrays.

};

#endif /* MYQ_H_ */
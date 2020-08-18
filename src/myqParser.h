#ifndef MYQPARSER_H_
#define MYQPARSER_H_

#include <Arduino.h>
#include <JsonHandler.h>

#include <forward_list>
#include <map>
#include <stdexcept>
#include <string>

char fullPath[200] = "";
char valueBuffer[50] = "";

// Structures to hold the parsed values
struct MyQ_account_t {
	String SecurityToken;
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

MyQ_devices_t MyQ_devices[5];  // up to 5 devices
MyQ_account_t MyQ_account;
// Example: LIST - Store a list of all forecasts
// std::forward_list<MyQ_devices_t> MyQ_devicesList;
// std::forward_list<MyQ_devices_t>::iterator MyQ_devicesList_it;

// Virtual Handler class implementation
class myqParser : public JsonHandler {
   private:
	// MyQ_devices_t MyQ_devices;

   public:
	/* Process the value provided at a specific JSON path / element
	 * This forms the core operation of the Handler.
	 */
	void value(ElementPath path, ElementValue value) {
		memset(fullPath, 0, sizeof(fullPath));
		path.toString(fullPath);
		const char* currentKey = path.getKey();
		Serial.print(fullPath);
		Serial.print("': ");
		Serial.println(value.toString(valueBuffer));

		const char* currentParent = getParent(fullPath);

		if (strcmp(currentKey, "SecurityToken") == 0) {
			MyQ_account.SecurityToken = value.getString();
			return;
		}

		else if (strcmp(currentParent, "Account") == 0) {
			if (strcmp(currentKey, "Id") == 0) {
				MyQ_account.Account_Id = value.getString();
				return;
			}
		}

		// Array items

		int currentIndex = getCurrentIndex(fullPath, "items");
		char items[10];
		sprintf(items, "items[%d]", currentIndex);
	
		if (strcmp(currentParent, items) == 0) {
			if (strcmp(currentKey, "serial_number") == 0) {
				MyQ_devices[currentIndex].serial_number = value.getString();
				return;
			} else if (strcmp(currentKey, "device_family") == 0) {
				MyQ_devices[currentIndex].device_family = value.getString();
				return;
			} else if (strcmp(currentKey, "device_platform") == 0) {
				MyQ_devices[currentIndex].device_platform = value.getString();
				return;
			} else if (strcmp(currentKey, "device_type") == 0) {
				MyQ_devices[currentIndex].device_type = value.getString();
				return;
			} else if (strcmp(currentKey, "name") == 0) {
				MyQ_devices[currentIndex].name = value.getString();
				return;
			} else if (strcmp(currentParent, "state") == 0) {
				if (strcmp(currentKey, "door_state") == 0) {
					MyQ_devices[currentIndex].state_door_state = value.getString();
					return;
				} else if (strcmp(currentKey, "last_update") == 0 || strcmp(currentKey, "updated_date") == 0) {
					MyQ_devices[currentIndex].state_last_update = value.getString();
					return;
				} else if (strcmp(currentKey, "online") == 0) {
					MyQ_devices[currentIndex].state_online = value.getBool();
					return;
				} else if (strcmp(currentKey, "last_status") == 0) {
					MyQ_devices[currentIndex].state_last_status = value.getString();
				}
			}
		}
	};

	void startDocument(){};
	void endDocument(){};
	void startObject(ElementPath path){};
	void endObject(ElementPath path){};
	void startArray(ElementPath path){};
	void endArray(ElementPath path){};
	void whitespace(char c){};

	const char* getParent(char* pathString) {
		char path[200];
		strcpy (path, pathString);
		const char* tok = ".";
		char* p;
		char* p2[10];
		int i = 0;
		
		p = strtok(path, tok);
		p2[0] = p;

		while (p != NULL) {
			p = strtok(NULL, tok);
			i++;
			p2[i] = p;
		}

		int element = i - 2;  // -1 to remove null entry at end of array, -2 to get second from last element

		if (i < 2) {  // only if token not found
			element = 0;
		}

		return p2[element];
	};
	
	int getCurrentIndex(char fullPath[], const char* arrayName) {
		char newArrayName[10];
		strcpy(newArrayName, arrayName);
		strcpy(newArrayName + strlen(arrayName), "[");
		char *target = NULL;
		char *start, *end;

	   	if (start = strstr(fullPath, newArrayName)) {
   	    	start += strlen(newArrayName);

   	  	  	if (end = strstr(start, "].")) {
   	      	  	target = (char*)malloc(end-start+1);
   	      	  	memcpy(target, start, end-start);
   	      	  	target[end-start] = '\0';
				int x = atoi(target);
				free(target);
				return x;
			}
		}
		return 0;
	};
};

#endif MYQPARSER_H_
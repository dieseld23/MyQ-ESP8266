#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecureBearSSL.h>
#include <myq.h>
// Fingerprint for https://api.myqdevice.com/api/v5/Login, expires on Aug 25, 2020
const uint8_t fingerprint[40] = {0x8a, 0xb1, 0x03, 0x3f, 0xd3, 0x12, 0x26, 0x89, 0x75, 0xcf, 0x03, 0x49, 0x74, 0x86, 0x25, 0x62, 0x15, 0x38, 0x74, 0x6d};

const String authVersion = "v5";
const String deviceVersion = "v5.1";
const String MyQApplicationId = "JVM/G9Nwih5BwKgNCjLxiFUQxQijAebyyg8QUHr7JOrP+tuPb8iHfRHKwTmDzHOu";
const String baseUrl = "https://api.myqdevice.com/api/";
const size_t bufferSize = 1024;
routes_t routes;
extern MyQ_devices_t MyQ_devices[10];				// up to 10 devices
extern MyQ_account_t MyQ_account;

MyQ::MyQ(String accountId, String username, String password, String securityToken) {
	_accountId = accountId;
	_username = username;
	_password = password;
	_securityToken = securityToken;
}

void MyQ::login(void) {
	if (_username == NULL || _password == NULL) {
		Serial.println("No username or password defined");
		// throw error, username and password not defined 	return ErrorHandler.returnError(14);
	}
	DynamicJsonDocument doc(bufferSize);
	String httpRequestData = "{\"Username\":\"" + _username + "\",\"Password\":\"" + _password + "\"}";
	doc = executeRequest(routes.login, "POST", httpRequestData);
	//Serial.println(doc.as<String>());
	//Serial.println(system_get_free_heap_size());
	//if (doc["returnCode"] != 0) {
		// throw error, look up in return code list        throw originalResponse;
	//}

	// if (!(doc["response"] || !(doc["response"])))
	//	  if (!response || !response.data) {
	//			return ErrorHandler.returnError(12);

	// const { data } = response;
	if (doc["SecurityToken"] == NULL || doc["SecurityToken"] == "") {
		Serial.println("No token");
		// return Error 11		return ErrorHandler.returnError(11);
	} 
	else {
		_securityToken = doc["SecurityToken"].as<String>();
	//	Serial.println(_securityToken);
	}
	doc.garbageCollect();
}

bool MyQ::checkIsLoggedIn() {
	if (_securityToken == NULL || _securityToken == "") {
		return false;
	}
	return true;
}

DynamicJsonDocument MyQ::executeRequest(String route, String method, String data) {
	bool isLoginRequest = false;
	int httpResponseCode = 0;
	
	DynamicJsonDocument doc(bufferSize);
	std::unique_ptr<BearSSL::WiFiClientSecure> client(new BearSSL::WiFiClientSecure);
	client->setFingerprint(fingerprint);

	HTTPClient https;

	String url = baseUrl;
	if (route == routes.login) {
		isLoginRequest = true;
		url += authVersion;
		url += route;
	} else if (route == routes.account) {  // expands account data
		url += authVersion;
		url += route;
		url += "?expand=account";
	} else {
		url += deviceVersion;
		url += route;
	}
	Serial.println(url);
	// https.setReuse(true);
	https.begin(*client, url);
	https.addHeader("Content-Type", "application/json");
	https.addHeader("MyQApplicationId", MyQApplicationId);
	https.addHeader("User-Agent", "okhttp/3.10.0");
	https.addHeader("ApiVersion", deviceVersion);
	https.addHeader("BrandId", "2");
	https.addHeader("Culture", "en");

	if (isLoginRequest == false && checkIsLoggedIn() == false) {  // If we aren't logged in or logging in, throw an error.
		Serial.println(F("not logged in or not logging in"));
		// return ErrorHandler.returnError(13);
	} else if (isLoginRequest == false) {
		https.addHeader("SecurityToken", _securityToken);  // Add our security token to the headers.
	}

	if (method == "POST") {
		Serial.println(data);
		httpResponseCode = https.POST(data);
	} 
	else if (method == "GET") {
		httpResponseCode = https.GET();
	} 
	else if (method == "PUT") {
		//return doc["returnCode"] = 32;	// return error, bad method
	}

	if (httpResponseCode > 0) {					// only if using GET or POST (not PUT)
		StaticJsonDocument<128> filter;
		if (route == routes.account) {
			filter["Account"]["Id"] = true;
			DeserializationError error = deserializeJson(doc, https.getString(),DeserializationOption::Filter(filter));			// need to filter, JSON returned from getDevices is too large and doesn't work 
			if (error) {
				Serial.println(F("ERROR: There was an error while deserializing1"));
				Serial.println(error.c_str());
				doc["returnCode"] = 31;
			}
		}

		else if (route == routes.getDevices) {
			filter["count"] = true;
			filter["items"]["serial_number"] = true;
			filter["items"]["device_family"] = true;
			filter["items"]["device_type"] = true;
			filter["items"]["name"] = true;
			filter["items"]["state"]["online"] = true;
			filter["items"]["state"]["door_state"] = true;
			filter["items"]["state"]["last_update"] = true;
			filter["items"]["state"]["last_status"] = true;

		}
	
		DeserializationError error = deserializeJson(doc, https.getString(),DeserializationOption::Filter(filter));	
		
		if (error) {
			Serial.println(F("ERROR: There was an error while deserializing"));
			Serial.println(error.c_str());
			doc["returnCode"] = 31;
			} 
		else {
			//parseData(doc, route);
			doc["returnCode"] = 0;
		}
			//return doc;
		//} else {
		//	doc["returnCode"] = 33;
			// parseBadResponse(httpResponseCode);
	//	}
	}
	https.end();  // Close connection
	return doc;
}

void MyQ::parseData(DynamicJsonDocument doc, String route) {

	if (route == routes.account) {				// commented out unused json items, left them to show what you can get if you need it
		//MyQ_account.Account_Id = doc["Account"]["Id"];
		//MyQ_account.Account_Name = doc["Account"]["Name"];
		//MyQ_account.Account_Email = doc["Account"]["Email"];
		//MyQ_account.Account_Address_AddressLine1 = doc["Account"]["Address"]["AddressLine1"];
		//MyQ_account.Account_Address_AddressLine2 = doc["Account"]["Address"]["AddressLine2"];
		//MyQ_account.Account_Address_City = doc["Account"]["Address"]["City"];
		//MyQ_account.Account_Address_State = doc["Account"]["Address"]["State"];
		//MyQ_account.Account_Address_PostalCode = doc["Account"]["Address"]["PostalCode"];
		//MyQ_account.Account_Address_Country_Code = doc["Account"]["Address"]["Country"]["Code"];
		//MyQ_account.Account_Address_Country_IsEEACountry = doc["Account"]["Address"]["Country"]["IsEEACountry"];
		//MyQ_account.Account_Phone = doc["Account"]["Phone"];
		//MyQ_account.Account_ContactName = doc["Account"]["ContactName"];
		//MyQ_account.Account_DirectoryCodeLength = doc["Account"]["DirectoryCodeLength"];
		//MyQ_account.Account_UserAllowance = doc["Account"]["UserAllowance"];
		//MyQ_account.Account_TimeZone = doc["Account"]["TimeZone"];
		//MyQ_account.AnalyticsId = doc["AnalyticsId"];
		MyQ_account.UserId = doc["UserId"];
		//MyQ_account.UserName = doc["UserName"];
		//MyQ_account.Email = doc["Email"];
		//MyQ_account.FirstName = doc["FirstName"];
		//MyQ_account.LastName = doc["LastName"];
		//MyQ_account.CultureCode = doc["CultureCode"];
		//MyQ_account.Address_PostalCode = doc["Address"]["PostalCode"];
		//MyQ_account.Address_Country_Code = doc["Address"]["Country"]["Code"];
		//MyQ_account.Address_Country_IsEEACountry = doc["Address"]["Country"]["IsEEACountry"];
		//MyQ_account.TimeZone_Id = doc["TimeZone"]["Id"];
		//MyQ_account.TimeZone_Name = doc["TimeZone"]["Name"];
		//MyQ_account.MailingListOptIn = doc["MailingListOptIn"];
		//MyQ_account.RequestAccountLinkInfo = doc["RequestAccountLinkInfo"];
		//MyQ_account.Phone = doc["Phone"];
		//MyQ_account.DiagnosticDataOptIn = doc["DiagnosticDataOptIn"];
	}
	else if (route == routes.getDevices) {
		for (int i=1; i <= doc["count"]; i++) {
		
			MyQ_devices[i].serial_number = doc["items"][i]["serial_number"]; 
			MyQ_devices[i].device_family = doc["items"][i]["device_family"];
			MyQ_devices[i].device_platform = doc["items"][i]["device_platform"];
			MyQ_devices[i].device_type = doc["items"][i]["device_type"]; 
			MyQ_devices[i].name = doc["items"][i]["name"]; 
			//MyQ_devices[i].created_date = doc["items"][i]["created_date"];
			//MyQ_devices[i].state_firmware_version = doc["items"][i]["state"]["firmware_version"];
			//MyQ_devices[i].state_homekit_capable = doc["items"][i]["state"]["homekit_capable"];
			//MyQ_devices[i].state_homekit_enabled = doc["items"][i]["state"]["homekit_enabled"];
			//MyQ_devices[i].state_learn = doc["items"][i]["state"]["learn"];
			//MyQ_devices[i].state_learn_mode = doc["items"][i]["state"]["learn_mode"];
			MyQ_devices[i].state_updated_date = doc["items"][i]["state"]["updated_date"];
			/*int jsonSize = doc["items"][i]["state"]["physical_devices"].size();
			if (jsonSize > 0) {
				for (int x = 0; x < jsonSize; x++) {
					MyQ_devices[i].state_physical_devices[x] = doc["items"][i]["state"]["physical_devices"][x]; 
				}
			}*/
			//MyQ_devices[i].state_pending_bootload_abandoned = doc["items"][i]["state"]["pending_bootload_abandoned"];
			MyQ_devices[i].state_online = doc["items"][i]["state"]["online"];
			MyQ_devices[i].state_last_status = doc["items"][i]["state"]["last_status"];
			//MyQ_devices[i].parent_device = doc["items"][i]["parent_device"]; 
			//MyQ_devices[i].parent_device_id = doc["items"][i]["parent_device_id"];
			//MyQ_devices[i].created_date = doc["items"][i]["created_date"];
			//MyQ_devices[i].state_gdo_lock_connected = doc["items"][i]["state"]["gdo_lock_connected"];
			//MyQ_devices[i].state_attached_work_light_error_present = doc["items"][i]["state"]["attached_work_light_error_present"];
			MyQ_devices[i].state_door_state = doc["items"][i]["state"]["door_state"]; 
			//MyQ_devices[i].state_open = doc["items"][i]["state"]["open"]; 
			//MyQ_devices[i].state_close = doc["items"][i]["state"]["close"];
			MyQ_devices[i].state_last_update = doc["items"][i]["state"]["last_update"];
			//MyQ_devices[i].state_passthrough_interval = doc["items"][i]["state"]["passthrough_interval"];
			//MyQ_devices[i].state_door_ajar_interval = doc["items"][i]["state"]["door_ajar_interval"];
			//MyQ_devices[i].state_invalid_credential_window = doc["items"][i]["state"]["invalid_credential_window"];
			//MyQ_devices[i].state_invalid_shutout_period = doc["items"][i]["state"]["invalid_shutout_period"];
			//MyQ_devices[i].state_is_unattended_open_allowed = doc["items"][i]["state"]["is_unattended_open_allowed"];
			//MyQ_devices[i].state_is_unattended_close_allowed = doc["items"][i]["state"]["is_unattended_close_allowed"];
			//MyQ_devices[i].state_aux_relay_delay = doc["items"][i]["state"]["aux_relay_delay"];
			//MyQ_devices[i].state_use_aux_relay = doc["items"][i]["state"]["use_aux_relay"];
			//MyQ_devices[i].state_aux_relay_behavior = doc["items"][i]["state"]["aux_relay_behavior"];
			//MyQ_devices[i].state_rex_fires_door = doc["items"][i]["state"]["rex_fires_door"];
			//MyQ_devices[i].state_command_channel_report_status = doc["items"][i]["state"]["command_channel_report_status"]; 
			//MyQ_devices[i].state_control_from_browser = doc["items"][i]["state"]["control_from_browser"];
			//MyQ_devices[i].state_report_forced = doc["items"][i]["state"]["report_forced"];
			//MyQ_devices[i].state_report_ajar = doc["items"][i]["state"]["report_ajar"];
			//MyQ_devices[i].state_max_invalid_attempts = doc["items"][i]["state"]["max_invalid_attempts"];
		}
	}
	else {
		// error of some sort, device type not found
	}
}

String MyQ::getAccountInfo() {
	String data = "";
	DynamicJsonDocument doc(bufferSize);
	doc = executeRequest(routes.account, "GET", data);
	_accountId = doc["Account"]["Id"].as<String>();
	
	if (/*doc["returnCode"] != 0 || */_accountId == NULL || _accountId == "") {
		Serial.println("No account info returned");
		//return ErrorHandler.returnError(11);
	} 
	return _accountId;
	doc.garbageCollect();
//.catch(({ response }) => ErrorHandler.parseBadResponse(response));

}

void MyQ::getDevices() {
	Serial.println(_accountId);
	if (_accountId == NULL || _accountId.isEmpty()) {
		getAccountInfo();
	}
	String newRoute = routes.getDevices;
	newRoute.replace("{accountId}", _accountId);
	Serial.println(newRoute);
	// if everything is ok with account info goto next step
	DynamicJsonDocument doc(bufferSize);
	doc = executeRequest(newRoute, "GET", "");

	if (doc["returnCode"] != 0 || doc["returnCode"] == NULL) {
		// error - no code returned
		Serial.println("no code returned");
	} 
	
	else {
	//	Serial.println(MyQ_devices[0].device_type);
	//	Serial.println(MyQ_devices[1].device_type);
	}
	
	doc.garbageCollect();
	/*
		  const {
			response: { data },
		  } = returnValue;

		  const devices = data.items;
		  if (!devices) {
			return ErrorHandler.returnError(11);
		  }

		  const result = {
			returnCode: 0,
		  };

		  const modifiedDevices = [];
		  Object.values(devices).forEach(device => {
			const modifiedDevice = {
			  family: device.device_family,
			  name: device.name,
			  type: device.device_type,
			  serialNumber: device.serial_number,
			};

			const { state } = device;
			if (constants.myQProperties.online in state) {
			  modifiedDevice.online = state[constants.myQProperties.online];
			}
			if (constants.myQProperties.doorState in state) {
			  modifiedDevice.doorState = state[constants.myQProperties.doorState];
			  const date = new Date(state[constants.myQProperties.lastUpdate]);
			  modifiedDevice.doorStateUpdated = date.toLocaleString();
			}
			if (constants.myQProperties.lightState in state) {
			  modifiedDevice.lightState = state[constants.myQProperties.lightState];
			  const date = new Date(state[constants.myQProperties.lastUpdate]);
			  modifiedDevice.lightStateUpdated = date.toLocaleString();
			}

			modifiedDevices.push(modifiedDevice);
		  });

		  result.devices = modifiedDevices;
		  return result;
		})
		.catch(({ response }) => ErrorHandler.parseBadResponse(response));
	}*/

}

void MyQ::getDeviceState() {
	/*
	getDeviceState(serialNumber, attributeName) {
	  return this.getDevices()
		.then(response => {
		  const device = (response.devices || []).find(d => d.serialNumber === serialNumber);
		  if (!device) {
			return ErrorHandler.returnError(18);
		  } else if (!(attributeName in device)) {
			return ErrorHandler.returnError(19);
		  }

		  const result = {
			returnCode: 0,
			state: device[attributeName],
		  };
		  return result;
		})
		.catch(({ response }) => ErrorHandler.parseBadResponse(response));
	}
	*/
}

void MyQ::getDoorState() {
	/*
	getDoorState(serialNumber) {
	  return this.getDeviceState(serialNumber, 'doorState')
		.then(result => {
		  if (result.returnCode !== 0) {
			return result;
		  }

		  const newResult = JSON.parse(JSON.stringify(result));
		  newResult.doorState = newResult.state;
		  delete newResult.state;
		  return newResult;
		})
		.catch(({ response }) => ErrorHandler.parseBadResponse(response));
	}*/
}

void MyQ::getLightState() {
	/*
		getLightState(serialNumber) {
	  return this.getDeviceState(serialNumber, 'lightState')
		.then(result => {
		  if (result.returnCode !== 0) {
			return result;
		  }

		  const newResult = JSON.parse(JSON.stringify(result));
		  newResult.lightState = newResult.state;
		  delete newResult.state;
		  return newResult;
		})
		.catch(({ response }) => ErrorHandler.parseBadResponse(response));
	}*/
}

void MyQ::setDeviceState() {
	/*setDeviceState(serialNumber, action) {
	  let promise = Promise.resolve();
	  if (!this.accountId) {
		promise = this.getAccountInfo();
	  }

	  return promise
		.then(() =>
		  this.executeRequest(
			constants.routes.setDevice
			  .replace('{accountId}', this.accountId)
			  .replace('{serialNumber}', serialNumber),
			'put',
			null,
			{ action_type: action }
		  )
		)
		.then(returnValue => {
		  const { returnCode } = returnValue;
		  if (returnCode !== 0 && typeof returnCode !== 'undefined') {
			return returnValue;
		  }

		  return {
			returnCode: 0,
		  };
		})
		.catch(({ response }) => ErrorHandler.parseBadResponse(response));
	}*/
}

void MyQ::setDoorOpen() {
	/*
	setDoorOpen(serialNumber, shouldOpen) {
	  let action = constants.doorCommands.close;

	  // Take a precaution and check against the string "false" so
	  // that someone doesn't inadvertently open their garage.
	  if (shouldOpen && shouldOpen !== 'false') {
		action = constants.doorCommands.open;
	  }
	  return this.setDeviceState(serialNumber, action);
	}
	*/
}

void MyQ::setLightState() {
	/*
	setLightState(serialNumber, turnOn) {
	  let action = constants.lightCommands.off;
	  if (turnOn) {
		action = constants.lightCommands.on;
	  }
	  return this.setDeviceState(serialNumber, action);
	}
	}*/
}
/*
result_t errorHandler::parseBadResponse(String response) {
	if (!response) {
	  return errorHandler::returnError(12, "", response);
	}

	const { data, status } = response;
	if (!status) {
	  return errorHandler::returnError(12, "", data);
	}
	if (status === 500) {
	  return errorHandler::returnError(15);
	}
	if ([400, 401].includes(status)) {
	  if (data.code === '401.205') {
		return errorHandler::returnError(16, "", data);
	  }
	  if (data.code === '401.207') {
		return errorHandler::returnError(17, "", data);
	  }
	  return errorHandler::returnError(14, "", data);
	}
	if (status === 404) {
	  // Return an error for a bad serial number.
	  if (data.code === '404.401') {
		return errorHandler::returnError(18, "", data);
	  }

	  // Handle generic 404 errors, likely indicating something wrong with this implementation.
	  return errorHandler::returnError(20);
	}

	return errorHandler::returnError(11, "", data);
  }



result_t errorHandler::returnError(int returnCode, String error, String response) {
	result.returnCode = returnCode;

	switch (returnCode) {
		case 11: result.message = errorMessages::e11; break;
		case 12: result.message = errorMessages::e12; break;
		case 13: result.message = errorMessages::e13; break;
		case 14: result.message = errorMessages::e14; break;
		case 15: result.message = errorMessages::e15; break;
		case 16: result.message = errorMessages::e16; break;
		case 17: result.message = errorMessages::e17; break;
		case 18: result.message = errorMessages::e18; break;
		case 19: result.message = errorMessages::e19; break;
		case 20: result.message = errorMessages::e19; break;
		default: result.message = "Unidentified message"; break;
	}
	result.providerMessage = "";
	result.unhandledError = "";

  //  if (response && response.description) {
  //    result.providerMessage = response.description;
 //   }
	if (error) {
	  result.unhandledError = error;
	}
	return result;

}



class ErrorHandler {
static parseBadResponse(response) {
  if (!response) {
	return ErrorHandler.returnError(12, null, response);
  }

  const { data, status } = response;
  if (!status) {
	return ErrorHandler.returnError(12, null, data);
  }
  if (status === 500) {
	return ErrorHandler.returnError(15);
  }
  if ([400, 401].includes(status)) {
	if (data.code === '401.205') {
	  return ErrorHandler.returnError(16, null, data);
	}
	if (data.code === '401.207') {
	  return ErrorHandler.returnError(17, null, data);
	}
	return ErrorHandler.returnError(14, null, data);
  }
  if (status === 404) {
	// Return an error for a bad serial number.
	if (data.code === '404.401') {
	  return ErrorHandler.returnError(18, null, data);
	}

	// Handle generic 404 errors, likely indicating something wrong with this implementation.
	return ErrorHandler.returnError(20);
  }

  return ErrorHandler.returnError(11, null, data);
}

static returnError(returnCode, error, response) {
  const result = {
	returnCode,
	message: constants.errorMessages[returnCode],
	providerMessage: null,
	unhandledError: null,
  };
  if (response && response.description) {
	result.providerMessage = response.description;
  }
  if (error) {
	result.unhandledError = error;
  }
  return Promise.resolve(result);
}
}

*/
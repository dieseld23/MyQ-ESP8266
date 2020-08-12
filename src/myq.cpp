#include <Arduino.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <JSON_Decoder.h>
#include <JSON_Listener.h>
#include <WiFiClientSecureBearSSL.h>
#include <myq.h>
// Fingerprint for https://api.myqdevice.com/api/v5/Login, expires on July 20, 2022
const uint8_t fingerprint[40] = {0x4f, 0x32, 0xcb, 0x4e, 0xbc, 0xdc, 0x7f, 0x19, 0xfd, 0x7e, 0x1f, 0xaf, 0x64, 0x01, 0x30, 0x5f, 0xe0, 0x9b, 0x21, 0xc6};

const String authVersion = "v5";
const String deviceVersion = "v5.1";
const String MyQApplicationId = "JVM/G9Nwih5BwKgNCjLxiFUQxQijAebyyg8QUHr7JOrP+tuPb8iHfRHKwTmDzHOu";
const String baseUrl = "https://api.myqdevice.com/api/";
routes_t routes;

MyQ::MyQ(String accountId, String username, String password, String securityToken) {
	_accountId = accountId;
	_username = username;
	_password = password;
	_securityToken = securityToken;
}

void MyQ::setLogin(String username, String password) {
	_username = username;
	_password = password;
}

void MyQ::login(void) {
	if (_username == NULL || _password == NULL) {
		Serial.println("No username or password defined");
		// throw error, username and password not defined 	return ErrorHandler.returnError(14);
	}
	String httpRequestData = "{\"Username\":\"" + _username + "\",\"Password\":\"" + _password + "\"}";
	getData(routes.login, "POST", httpRequestData);

	if (_securityToken == NULL || _securityToken == "") {
		Serial.println("Login: No token");
		// return Error 11		return ErrorHandler.returnError(11);
	} else {
		Serial.println(_securityToken);
	}
}

bool MyQ::checkIsLoggedIn() {
	if (_securityToken == NULL || _securityToken == "") {
		return false;
	}
	return true;
}

String MyQ::getAccountInfo() {
	getData(routes.account, "GET");

	Serial.println(_accountId);
	if (_accountId == NULL || _accountId == "") {
		Serial.println("No account info returned");
		// return ErrorHandler.returnError(11);
	}
	return _accountId;
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
	getData(newRoute, "GET", "");
	// if (doc["returnCode"] != 0 || doc["returnCode"] == NULL) {
	// error - no code returned
	//		Serial.println("no code returned");
	//}

	//	else {
	//	Serial.println(MyQ_devices[0].device_type);
	//	Serial.println(MyQ_devices[1].device_type);
	//	}

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

void MyQ::getData(String route, String method, String data) {
	JSON_Decoder parser;	   // Create an instance of the parser
	parser.setListener(this);  // Pass pointer to "this" MyQ class to the listener
							   // so it can call the support functions in this class
	bool isLoginRequest = false;
	int httpResponseCode = 0;
	
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

	//https.useHTTP10(true);
	https.begin(*client, url);
	https.addHeader("Content-Type", "application/json");
	https.addHeader("MyQApplicationId", MyQApplicationId);
	https.addHeader("User-Agent", "okhttp/3.10.0");
	https.addHeader("ApiVersion", deviceVersion);
	https.addHeader("BrandId", "2");
	https.addHeader("Culture", "en");

	if (isLoginRequest == false && checkIsLoggedIn() == false) {  // If we aren't logged in or logging in, throw an error.
		Serial.println(F("Not logged in OR not logging in"));
		// return ErrorHandler.returnError(13);
	} else if (isLoginRequest == false) {
		Serial.println("added SecurityToken: " + _securityToken);
		https.addHeader("SecurityToken", _securityToken);  // Add our security token to the headers.
	}
	if (method == "POST") {
		Serial.println(data);
		httpResponseCode = https.POST(data);
	} else if (method == "GET") {
		httpResponseCode = https.GET();
	} else if (method == "PUT") {
		// return doc["returnCode"] = 32;	// return error, bad method
	} else {
		Serial.println(F("No method given for HTTP Request"));
		return;
	}

	Serial.println(httpResponseCode);
	// Local variables for time-out etc
	uint32_t timeout = millis();
	char c = 0;
	//uint16_t count = 0;

	// Read the JSON character by character and pass it to the JSON decoder
	// The decoder will call the MyQ class during decoding, so we can save the decoded values

	// Use OR since data may still be in the buffer when the client has disconnected!
	while (client->available() > 0/* || client->connected()*/) {
		while (client->available() > 0) {
			c = client->read();	 // Read a received character

			parser.parse(c);  // Pass to the parser, parser will call listener support functions as needed
			if ((millis() - timeout) > 250UL) {  // Check for timeout
				Serial.println("JSON parse client timeout");
				parser.reset();
				https.end();
				client->stop();
				return;
			}
			
		}
		

	}

	Serial.println("end");
	parser.reset();
	
	client->stop();
	https.end();
}

/***************************************************************************************
**  JSON Decoder library calls this when a key has been read
***************************************************************************************/
void MyQ::key(const char *key) {
	currentKey = key;

	// Serial.print("key: ");
	 //Serial.println(key);
}

/***************************************************************************************
**  JSON Decoder library calls this when a value has been read
***************************************************************************************/
void MyQ::value(const char *value) {
	String val = value;

	// Test only:
	// Serial.print("\nvaluePath     :"); Serial.println(valuePath);
	// Serial.print("currentParent :"); Serial.println(currentParent);
	// Serial.print("currentKey    :"); Serial.println(currentKey);
	// Serial.print("arrayIndex    :"); Serial.println(arrayIndex);
	// Serial.print("Value    :"); Serial.println(val);

	if (currentKey == "SecurityToken") {
		_securityToken = val;
		return;
	}

	if (currentParent == "Account") {
		if (currentKey == "Id") {
			//MyQ_account->Account_Id = val;
			_accountId = val;
			return;
		}
	}

	else if (currentParent == "Devices") {
		if (currentKey == "serial_number") {
			MyQ_devices->serial_number = val;
			return;
		} else if (currentKey == "device_family") {
			MyQ_devices->device_family = val;
			return;
		} else if (currentKey == "device_platform") {
			MyQ_devices->device_platform = val;
			return;
		} else if (currentKey == "device_type") {
			MyQ_devices->device_type = val;
			return;
		} else if (currentKey == "name") {
			MyQ_devices->name = val;
			return;
		} else if (currentKey == "state_updated_date") {
			MyQ_devices->state_updated_date = val;
			return;
		} else if (currentKey == "state_door_state") {
			MyQ_devices->state_door_state = val;
			return;
		} else if (currentKey == "state_last_update") {
			MyQ_devices->state_last_update = val;
			return;
		} else if (currentKey == "state_online") {
			//MyQ_devices->state_online = (bool)val;
			return;
		}
	}
}
/***************************************************************************************
**  JSON Decoder library calls this when a start of document decoded
***************************************************************************************/
void MyQ::startDocument() {
	currentParent = currentKey = "";
	arrayIndex = 0;
	ended = false;
	 //Serial.println("\nstart document");
}

/***************************************************************************************
**  JSON Decoder library calls this when a end of document decoded
***************************************************************************************/
void MyQ::endDocument() {
	ended = true;
	// Serial.println("end document. ");
}

/***************************************************************************************
**  JSON Decoder library calls this when a start of object decoded
***************************************************************************************/
void MyQ::startObject() {
	currentParent = currentKey;
	// Serial.println("start object. ");
}

/***************************************************************************************
**  JSON Decoder library calls this when a end of object decoded
***************************************************************************************/
void MyQ::endObject() {
	currentParent = "";
	arrayIndex++;
	 //Serial.println("end object. Array Index: " + arrayIndex);
}

/***************************************************************************************
**  JSON Decoder library calls this when an array of values has started
***************************************************************************************/
void MyQ::startArray() {
	arrayIndex = 0;
	valuePath = currentParent + "/" + currentKey;
	// Serial.println("start array. ");
}

/***************************************************************************************
**  JSON Decoder library calls this when an array of values has ended
***************************************************************************************/
void MyQ::endArray() {
	valuePath = "";
	 //Serial.println("end array. ");
}

/***************************************************************************************
**  JSON Decoder library calls this when a character is whitespace (not used here)
***************************************************************************************/
// whitespace(char c) not used, JSON Listener substitutes a dummy function
// void MyQ::whitespace(char c) {
// Serial.println("whitespace");
//}

/***************************************************************************************
**  JSON Decoder library calls this when a decoding error occurs
***************************************************************************************/
void MyQ::error(const char *message) {
	if (ended == false)	 // Only report errors in a document
	{
		Serial.print("\nError message: ");
		Serial.println(message);
	}
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
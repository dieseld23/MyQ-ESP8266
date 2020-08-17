#include <Arduino.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <ArduinoStreamParser.h>
#include <WiFiClientSecureBearSSL.h>
#include <myq.h>
#include <myqParser.h>

#include <forward_list>
#include <stdexcept>

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
	}
	String httpRequestData = "{\"Username\":\"" + _username + "\",\"Password\":\"" + _password + "\"}";
	getData(routes.login, "POST", httpRequestData);

	if (!checkIsLoggedIn()) {
		Serial.println("Login: No token");
		// return Error 11		return ErrorHandler.returnError(11);
	} else {
		Serial.println("Login Successful");
	}
}

bool MyQ::checkIsLoggedIn() {
	if (MyQ_account.SecurityToken == NULL || MyQ_account.SecurityToken == "") {
		return false;
	}
	return true;
}

bool MyQ::getAccountInfo() {
	if (MyQ_account.Account_Id == NULL || MyQ_account.Account_Id.isEmpty()) {
		getData(routes.account, "GET");

		if (MyQ_account.Account_Id == NULL || MyQ_account.Account_Id.isEmpty()) {
			Serial.println("No account info returned");
			return false;
			// return ErrorHandler.returnError(11);
		}
		Serial.println("Account info returned");
		return true;
	}
	Serial.println("No account info returned");
	return false;
}

void MyQ::getData(String route, String method, String data) {
	ArudinoStreamParser parser;	 // JSON Streaming Parser
	myqParser myqHandler;

	parser.setHandler(&myqHandler);	 // Link to customer listener (parser to be honest) // Pass pointer to "this" MyQ class to the listener

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

	// https.useHTTP10(true);
	https.begin(*client, url);
	https.addHeader("Content-Type", "application/json");
	https.addHeader("MyQApplicationId", MyQApplicationId);
	https.addHeader("User-Agent", "okhttp/3.10.0");
	https.addHeader("ApiVersion", deviceVersion);
	https.addHeader("BrandId", "2");
	https.addHeader("Culture", "en");

	if (isLoginRequest == false && checkIsLoggedIn() == false) {  // If we aren't logged in or logging in, throw an error.
		Serial.println(F("Not logged in OR not logging in"));
	} else if (isLoginRequest == false) {
		Serial.println(MyQ_account.SecurityToken);
		https.addHeader("SecurityToken", MyQ_account.SecurityToken);  // Add our security token to the headers.
	}
	if (method == "POST") {
		Serial.println("POST");
		httpResponseCode = https.POST(data);
	} else if (method == "GET") {
		Serial.println("GET");
		Serial.println("Account Id:  " + MyQ_account.Account_Id);
		httpResponseCode = https.GET();
	} else if (method == "PUT") {
		// return doc["returnCode"] = 32;	// return error, bad method
	} else {
		Serial.println(F("No method given for HTTP Request"));
		return;
	}

	Serial.println("Parsing JSON...");
	https.writeToStream(&parser);  // Shoot it straight to the parser
	Serial.println("Completed Parsing.");
	client->stop();
	https.end();
}

void MyQ::getDevices() {
	if (getAccountInfo()) {
		String newRoute = routes.getDevices;
		newRoute.replace("{accountId}", MyQ_account.Account_Id);
		// if everything is ok with account info goto next step
		getData(newRoute, "GET", "");
	}
}

/*const char* MyQ::getDeviceState(const char* serial_number, const char* attributeName) {

	for (int i = 0; i < _count; i++) {
		if (strcmp(MyQ_devices[_count]->serial_number, serial_number)) {
			if (strcmp(attributeName, MyQ_devices[_count]->name)) {
				return MyQ_devices[_count]->name;
			}
			else if ((strcmp(attributeName, MyQ_devices[_count]->name)) {
			}
		}
	}


}*/

void MyQ::getDoorState() {
}

void MyQ::getLightState() {
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

///
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
void MyQ::printHeapFreeToSerial() {
	Serial.print("Heap free: ");
	Serial.println(String(ESP.getFreeHeap(), DEC));
	Serial.print("Stack free: ");
	Serial.println(String(ESP.getFreeContStack(), DEC));
}
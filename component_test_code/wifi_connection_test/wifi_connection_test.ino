#include <WiFi.h>
#include <HTTPClient.h>
#include "secrets.h"

// Secret.h
// char ssid[] = "secret";
// char pass[] = "secret";

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  //Configure a timeout that terminates the connection attempt after 10 seconds
  int timeoutCounter = 0;
  const int timeoutLimit = 20; 

  while (WiFi.status() != WL_CONNECTED && timeoutCounter < timeoutLimit) {
    delay(500);
    Serial.print(".");
    timeoutCounter++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println();
    Serial.println("WiFi connected!");

    //Print out the IP address
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    //Check the signal strength
    Serial.print("Signal strength (RSSI): ");
    Serial.println(WiFi.RSSI());

    //Call function that makes a test request
    testHttpRequest();

  } else {
    //ESP32 failed to connect to the internet
    Serial.println();
    Serial.println("Failed to connect to WiFi. Check SSID/password and try again.");
  }
}

//Create a function that makes a test call to http://httpbin.org/get --> important for later gadget functionalities
void testHttpRequest() {
  //Object that manages the network request
  HTTPClient http;

  Serial.println();
  Serial.println("Sending test HTTP request to httpbin.org...");

  //Configure the request target and prepare the connection
  http.begin("http://httpbin.org/get");

  //Sends the request over the network and waits for a response --> returns an int
  //  Positive number: HTTP status code the server sent back (200--> success, 4xx/5xx--> error)
  //  Negative number: Request never got a response (client-side failure)
  int httpCode = http.GET();

  //if the request went through, print the JSON describing the request
  if (httpCode > 0) {
    Serial.print("HTTP response code: ");
    Serial.println(httpCode);

    String payload = http.getString();
    Serial.println("Response body:");
    Serial.println(payload);

    //else, an error has occured, print the error code
  } else {
    Serial.print("HTTP request failed, error: ");
    Serial.println(http.errorToString(httpCode).c_str());
  }

  //close the connection and free memory/resources from the HTTPClient
  http.end();
}

void loop() {
  // put your main code here, to run repeatedly:

}

#ifndef BACKEND_H
#define BACKEND_H

#include <WiFi.h>
#include <HTTPClient.h>
#include "secrets.h"
#include <ArduinoJson.h>

// Secret.h
// char ssid[] = "secret";
// char pass[] = "secret";
// char http[] = "secret";

//Calendar block data
extern String next_event;
extern String event_start;
extern String event_location;

//Leave_by block data
extern String leave_by;
extern String time_remain;

//Weather data
extern String temp_c;
extern String condition;
extern String condition_note;
extern String weather_note;

//Transit block data
extern String route_summary;
extern String travel_duration;


void connectWifi();
String httpRequest();
bool parseJson(String data);

#endif
#include "backend.h"

//Calendar block data
String next_event;
String event_start;
String event_location;

//Leave_by block data
String leave_by;
String time_remain;

//Weather data
String temp_c;
String condition;
String condition_note;
String weather_note;

//Transit block data
String route_summary;
String travel_duration;

void connectWifi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
 
  Serial.print("Connecting to WiFi");
 
  const unsigned long WIFI_TIMEOUT_MS = 10000; // 10 seconds
  unsigned long startAttemptTime = millis();
 
  while (WiFi.status() != WL_CONNECTED) {
    if (millis() - startAttemptTime >= WIFI_TIMEOUT_MS) {
      Serial.println();
      Serial.println("WiFi connection timed out. Restarting device...");
      ESP.restart();
    }
    delay(500);
    Serial.print(".");
  }
 
  Serial.println();
  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());
}

String httpRequest() {
  String payload = "";

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected, skipping request");
    return payload;
  }

  HTTPClient httpClient;
  httpClient.begin(httpAddress); // "http" from secrets.h — endpoint URL
  httpClient.addHeader("X-Device-Key", device_key);


  int httpCode = httpClient.GET();

  if (httpCode == HTTP_CODE_OK) {
    payload = httpClient.getString();
  } else {
    Serial.print("HTTP request failed, code: ");
    Serial.println(httpCode);
  }

  httpClient.end();
  return payload;
}

bool parseJson(String data) {
  if (data.length() == 0) {
    return false;
  }

  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, data);

  if (error) {
    Serial.print("JSON parse failed: ");
    Serial.println(error.c_str());
    return false;
  }

  // Calendar block
  next_event     = doc["next_event"] | "unknown";
  event_start    = doc["event_start"] | "unknown";
  event_location = doc["event_location"] | "unknown";
 
  // Leave-by block
  leave_by    = doc["leave_by"] | "unknown";
  time_remain = doc["time_remain"] | "unknown";
 
  // Weather block
  temp_c          = doc["temp_c"] | "unknown";
  condition       = doc["condition"] | "unknown";
  condition_note  = doc["condition_note"] | "unknown";
  weather_note    = doc["weather_note"] | "unknown";
 
  // Transit block
  route_summary   = doc["route_summary"] | "unknown";
  travel_duration = doc["travel_duration"] | "unknown";
 
  return true;
}
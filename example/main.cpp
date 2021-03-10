#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <MQTTSensor.h>

const char* ssid = "BT-SCAKPC";
const char* password = "btvxqTve4aPQkr";

WiFiClient wifiClient;
MQTTSensor Sensor1(5, "trains/track/sensor/456", wifiClient);
MQTTSensor Sensor2(4, "trains/track/sensor/789", wifiClient);

// Forward declarations.
void connectToWiFi();

void setup() {
  Serial.begin(115200);

  connectToWiFi();

  Sensor1.setDebounceDelay_mS(100);
  //Sensor1.setStartupTopic("events");
}

void loop() {
  Sensor1.update();
  Sensor2.update();
}

void connectToWiFi() {
  Serial.print("\n\nConnecting to WiFi ");

  WiFi.begin(ssid, password);
  
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

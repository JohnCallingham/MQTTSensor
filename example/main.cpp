#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <MQTTSensor.h>
#include <MQTTSensorContainer.h>
#include <WebSocketsServer.h>

const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

WiFiClient wifiClient;
PubSubClient mqttClient;
WebSocketsServer webSocket(81);

MQTTSensorContainer container;
MQTTSensor* sensor1 = container.addSensor(5, "trains/track/sensor/456");
MQTTSensor* sensor2 = container.addSensor(4, "trains/track/sensor/789");

// Forward declarations.
void connectToWiFi();

void setup() {
  Serial.begin(115200);

  connectToWiFi();

  webSocket.begin();

  container.setStartupTopic("events"); // Sets the startup topic for the container and all sensors.
  sensor1->setDebounceDelay_mS(5);
  sensor2->setDebounceDelay_mS(5);
}

void loop() {
  webSocket.loop();
  container.loop(); // Calls the loop method of all MQTTSensor objects which have been added to the container.
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

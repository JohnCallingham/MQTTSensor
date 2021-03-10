#include <Arduino.h>
#include <MQTTSensor.h>
#include <ESP8266WiFi.h>

MQTTSensor::MQTTSensor(uint8_t pinNumber, const char* sensorTopic, WiFiClient& wifiClient) {
    // Store the parameters.
    this->pinNumber = pinNumber;
    this->sensorTopic = sensorTopic;
    this->wifiClient = wifiClient;

    // Set the default values.
    this->mqttBroker = "raspberrypi";
    this->mqttPort = 1883;
    this->startupTopic = "events";
    this->debounceDelay_mS = 50;

    // Configure the sensor pin.
    pinMode(this->pinNumber, INPUT_PULLUP);
}

void MQTTSensor::setBroker(const char* mqttBroker) {
    this->mqttBroker = mqttBroker;
}

void MQTTSensor::setPort(uint16_t mqttPort) {
    this->mqttPort = mqttPort;
}

void MQTTSensor::setStartupTopic(const char* startupTopic) {
    this->startupTopic = startupTopic;
}

void MQTTSensor::setDebounceDelay_mS(unsigned long debounceDelay_mS) {
    this->debounceDelay_mS = debounceDelay_mS;
}

void MQTTSensor::connectToMQTT() {
    char mqtt_client_id[20];
    char startupMessage[100];

    Serial.println ("\nConnecting to MQTT.");

    sprintf(mqtt_client_id, "esp8266-%x-%i", ESP.getChipId(), this->pinNumber);

    mqttClient.setClient(this->wifiClient);
    mqttClient.setServer(this->mqttBroker, this->mqttPort);
    mqttClient.setBufferSize(255);

    // Loop until connected.
    while (!mqttClient.connected()) {
        if (mqttClient.connect(mqtt_client_id)) {
            Serial.printf("Connected to MQTT broker: %s\n", mqttBroker);

            // Publish a message to show that we have started OK.
            sprintf(startupMessage, "MQTTSensor started on pin %i. Publishing to topic %s.", this->pinNumber, this->sensorTopic);

            // Show the startup message on the Serial port.
            Serial.printf("Publishing startup message to topic '%s':\n", this->startupTopic);
            Serial.printf("Startup message: %s\n", startupMessage);

            // Publish the startup message to the MQTT broker.
            mqttClient.publish(this->startupTopic, startupMessage);

        } else {
        Serial.print("failed, rc=");
        Serial.print(mqttClient.state());
        Serial.println(" try again in 5 seconds");
        // Wait 5 seconds before retrying.
        delay(5000);
        }
    }
}

void MQTTSensor::update() {
    // Connect to MQTT if not done before.
    if (!this->mqttClient.connected()) {
        connectToMQTT();
    }

    // Update the MQTT client.
    this->mqttClient.loop();

    // Read the sensor pin and publish the appropriate message.
    int readState;

    // Read the level on pinNumber.
    readState = digitalRead(this->pinNumber);

    // Has the pin level changed due to bounce or otherwise?
    if (readState != this->lastState) {
        // Yes, so reset the debounce timer.
        this->lastTime = millis();
    }

    // Has time moved on from the last level change by the debounce delay?
    if ((millis() - this->lastTime) > this->debounceDelay_mS) {
        // Whatever the current level, it has been stable for the debounce delay.
        // So check for a change in level.
        if (readState != this->currentState) {
            // There has been a debounced change in level so store the new level.
            this->currentState = readState;

            // Publish the change to the MQTT broker.
            if (this->currentState == HIGH) {
                publishMQTTSensor(this->sensorTopic, "INACTIVE");
            } else {
                publishMQTTSensor(this->sensorTopic, "ACTIVE");
            }
        }
    }

    this->lastState = readState;
}

void MQTTSensor::publishMQTTSensor(const char* topic, const char* payload) {

  // Publish the payload to the sensor topic. Retained is set to False.
  this->mqttClient.publish(topic, payload, false);

  Serial.printf("Message published to topic [%s]  %s\n", topic, payload);
}

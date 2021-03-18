#ifndef MQTT_SENSOR_H
#define MQTT_SENSOR_H

#include <PubSubClient.h>
#include <WebSocketsServer.h>

class MQTTSensor {
    public:
        MQTTSensor(uint8_t pinNumber, const char* sensorTopic);

        void loop();

        void setDebounceDelay_mS(unsigned long debounceDelay_mS) {this->debounceDelay_mS = debounceDelay_mS;}

        uint8_t getPinNumber() {return this->pinNumber;}
        const char* getSensorTopic() {return this->sensorTopic;}

        void publishMQTTSensor();
        
    private:
        uint8_t pinNumber;
        const char* mqttBroker = "raspberrypi";
        uint16_t mqttPort = 1883;

        const char* sensorTopic;

        void connectToMQTT();
        void readPin();

        unsigned long debounceDelay_mS = 50;
        int currentState;
        int lastState;
        unsigned long lastTime;

};
#endif

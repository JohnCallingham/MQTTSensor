#ifndef MQTT_SENSOR_CONTAINER_H
#define MQTT_SENSOR_CONTAINER_H

#include <MQTTSensor.h>
#include <list>
#include <ESP8266WebServer.h>

class MQTTSensorContainer {

    public:
        MQTTSensorContainer();

        MQTTSensor* addSensor(uint8_t pinNumber, const char* sensorTopic);
        void loop();

        void setBroker(const char* mqttBroker) {this->mqttBroker = mqttBroker;}
        void setPort(uint16_t mqttPort) {this->mqttPort = mqttPort;}
        void setStartupTopic(const char* startupTopic) {this->startupTopic = startupTopic;}

    private:
        const char* ssid;
        const char* password;
        std::list<MQTTSensor*> sensorList;
        //std::list<MQTTSensor*>::iterator it;

        String webpage = "";

        const char* mqttBroker = "raspberrypi";
        uint16_t mqttPort = 1883;
        const char* startupTopic = "events";

        void connectToMQTT();
        void buildWebPage();
        void sendStartupMessage();
        String replaceAll(String s);
        String getSensorsJSON();

        ESP8266WebServer server;

        int connectedClients;
        bool initialised = false;

};
#endif

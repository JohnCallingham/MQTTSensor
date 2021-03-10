#include <ESP8266WiFi.h>
#include <PubSubClient.h>

class MQTTSensor {
    public:
        MQTTSensor(uint8_t pinNumber, const char* sensorTopic, WiFiClient& wifiClient);

        void update();

        void setBroker(const char* mqttBroker);
        void setPort(uint16_t mqttPort);
        void setStartupTopic(const char* startupTopic);
        void setDebounceDelay_mS(unsigned long debounceDelay_mS);

    private:
        WiFiClient wifiClient;
        PubSubClient mqttClient;
        uint8_t pinNumber;
        const char* mqttBroker;
        uint16_t mqttPort;


        const char* sensorTopic;
        const char* startupTopic;

        void connectToMQTT();
        void publishMQTTSensor(const char* topic, const char* payload);

        unsigned long debounceDelay_mS;
        int currentState;
        int lastState;
        unsigned long lastTime;
};

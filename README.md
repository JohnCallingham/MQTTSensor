# MQTTSensor
A library to interface sensors to JMRI using MQTT. Designed to run on a Wemos D1 Mini.
## Functions
* The basic functionality is to monitor one or more input pins and to publish an MQTT message if there is a change in state of the pin. JMRI can be configured to monitor MQTT messages and set its sensors accordingly.
* A web server is included which displays a web page which shows information about each sensor input.
* When the device starts it publishes a JSON startup message to an MQTT topic. The topic defaults to "events", but can be overwritten in the code.
## Library Dependants
* Nick O'Leary's [PubSubClient libary](https://github.com/knolleary/pubsubclient/) is required to handle the MQTT communications.
* Markus Sattler's [WebSockets library](https://github.com/Links2004/arduinoWebSockets) is required to update the web page when an input changes state.
## Configurable items in SensorContainer
* setBroker. This allows the user to configure the name of the MQTT broker. Defaults to "raspberrypi".
* setPort. This allows the user to configure the MQTT port. Defaults to 1883.
* setStartupTopic. Allows the MQTT topic to be set which is published on when the device starts running. Defaults to "events".
## Configurable items in Sensor
* setDebounceDelay_mS. Allows configuration of the debounce delay. Defaults to 50mS.

#include <MQTTSensorContainer.h>
#include <ESP8266WiFi.h>
#include <MQTTSensor.h>
#include <list>
#include <ESP8266WebServer.h>

extern WebSocketsServer webSocket;
extern WiFiClient wifiClient;
extern PubSubClient mqttClient;

MQTTSensorContainer::MQTTSensorContainer() {
    // Start the web server.
    this->server.on ("/", [&]() {this->server.send(200, "text/html", this->webpage);});
    this->server.begin();

    // Initialise the number of connected web socket clients.
    this->connectedClients = 0;
}

MQTTSensor* MQTTSensorContainer::addSensor(uint8_t pinNumber, const char* sensorTopic) {
    // Use new to create the new MQTTSensor object on the heap so it is not destroyed when this function returns.
    // https://stackoverflow.com/questions/15310846/creating-a-class-object-in-c
    MQTTSensor* newSensor = new MQTTSensor(pinNumber, sensorTopic);

    // Add the new sensor to the end of the sensor list.
    sensorList.push_back(newSensor);

    // Return the pointer to the new sensor.
    return newSensor;
}

void MQTTSensorContainer::loop() {
    if (!initialised) {
        this->connectToMQTT();

        // Send startup message to MQTT broker.
        this->sendStartupMessage();

        this->buildWebPage();

        this->initialised = true;
    }

    // Update the MQTT client. This is needed to prevent the connection from dropping.
    mqttClient.loop();
    
    // Iterate over all pointer to MQTTSensor objects in the sensor list calling their loop() method.
    // // Using the traditional for loop with an iterator.
    // for (it = sensorList.begin(); it != sensorList.end(); it++) {
    //     (*it)->update();
    // }
    // Using a range-based for loop.
    for (MQTTSensor* sensor : sensorList) {
		sensor->loop();
	}

    server.handleClient();

    // Check to see if a new client has connected to the web socket.
    // If one has, then update the sensor state values in the browser.
    if (webSocket.connectedClients(true) != connectedClients) {
        connectedClients = webSocket.connectedClients(true);

        // Update the web page with the current state of all sensors.
        for (MQTTSensor* sensor : sensorList) {
            sensor->publishMQTTSensor();
        }
    }
}

void MQTTSensorContainer::connectToMQTT() {
    char mqtt_client_id[20];

    Serial.println ("\nConnecting to MQTT.");

    sprintf(mqtt_client_id, "esp8266-%x", ESP.getChipId());

    Serial.printf ("mqtt_client_id %s\n", mqtt_client_id);

    //Serial.printf("%s\n", wifiClient.localIP().toString().c_str());

    mqttClient.setClient(wifiClient);
    mqttClient.setServer(this->mqttBroker, this->mqttPort);
    mqttClient.setBufferSize(1024); // Increased from the default.

    // Loop until connected.
    while (!mqttClient.connected()) {
        if (mqttClient.connect(mqtt_client_id)) {
            Serial.printf("Connected to MQTT broker: %s\n", mqttBroker);
        } else {
        Serial.print("failed, rc=");
        Serial.print(mqttClient.state());
        Serial.println(" try again in 5 seconds");
        // Wait 5 seconds before retrying.
        delay(5000);
        }
    }
}

void MQTTSensorContainer::buildWebPage() {
    const char HomePageHTML[] = R"rawliteral(
    <!DOCTYPE HTML>
    <html>
    <head>
    <title>MQTT Interface</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style type = "text/css"> 
        .textinput { 
        width: 100px;
        background-color: Gold;
        }
        .textreadonly {
        width: 100px;
        background-color: White;
        }
        .texthidden {
        display: none;
        }
        table {
        border: 1px solid blue;
        border-spacing: 10px;
        }
        td {
        vertical-align: middle;
        border-bottom: 1px solid #ddd;
        }
    </style> 

    <script>
        var Socket;
        function init() {
            Socket = new WebSocket('ws://' + window.location.hostname + ':81/');
            Socket.onmessage = function(event) {
                console.log('message received');
                document.getElementById(event.data[0]).innerHTML = event.data.slice(1);
            }
            Socket.onopen = function(event) {console.log('Connection opened');}
            Socket.onerror = function(event) {console.log('Error');}
        }
    </script>
       

    )rawliteral";


    webpage += HomePageHTML;

    webpage += "</head>";
    webpage += "<body onload='javascript:init()'>";

    webpage += "<table>\n";

    webpage += "<tr>";
    webpage += "<td>Sensor Pin</td>";
    webpage += "<td>Sensor Topic</td>";
    webpage += "<td>Sensor State</td>";
    webpage += "</tr>\n";
    
    // Add a table row for each sensor.
    for (MQTTSensor* sensor : sensorList) {
        webpage += "<tr>";
        webpage += "<td>";
		webpage += String(sensor->getPinNumber());
        webpage += "</td>";
        webpage += "<td>";
		webpage += String(sensor->getSensorTopic());
        webpage += "</td>";
        webpage += "<td><div id='";
        webpage += sensor->getPinNumber();
        webpage += "'></div>";
        webpage += "</td>";
        webpage += "</tr>\n";
	}

    webpage += "</table>";
    webpage += "</body>";
    webpage += "</html>";
}

void MQTTSensorContainer::sendStartupMessage() {
    const char startupMessage[] = R"rawliteral(
        {
            "Event Name": "Device startup",
            "Device": {
                "Name": "MQTT Sensor",
                "ID": "%ID%",
                "IP": "%IP%"
            },
            "Sensors": [
                %SENSORS%
            ]
        }
        )rawliteral";

    String message(startupMessage);

    message = replaceAll(message);

    char c[message.length() + 1];
    strcpy(c, message.c_str());

    mqttClient.publish(this->startupTopic, c);
    //Serial.println(c);

}

String MQTTSensorContainer::replaceAll(String s) {
    char chipID[10];
    sprintf(chipID, "%04x", ESP.getChipId());
    s.replace("%ID%", chipID);
    s.replace("%IP%", WiFi.localIP().toString());
    s.replace("%SENSORS%",  getSensorsJSON());
    return s;
}

String MQTTSensorContainer::getSensorsJSON() {
    String retValue = "";

   for (MQTTSensor* sensor : sensorList) {
       retValue += "{\"Pin\": \"";
       retValue += sensor->getPinNumber();
       retValue += "\", \"Topic\": \"";
       retValue += sensor->getSensorTopic();
       retValue += "\"},\n";
	}

    // Remove last ",".
    retValue = retValue.substring(0, retValue.length() - 2);

    return retValue;
}

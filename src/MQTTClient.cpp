#include "MQTTClient.h"
#include <utility>

MQTTClient* MQTTClient::instance = nullptr;

MQTTClient::MQTTClient(const char* mqtt_server)
    : mqtt_server(mqtt_server), client(espClient) {
        instance = this;
    }

void MQTTClient::begin(std::function<void(const String&)> mainCallback, const char* mqtt_username, const char* mqtt_password) {
    this->mqtt_username = mqtt_username;
    this->mqtt_password = mqtt_password;
    client.setServer(mqtt_server, 1883);
    client.setCallback(callback);
    connectMQTT();
    messageCallback = std::move(mainCallback);
}

void MQTTClient::connectMQTT() {
    Serial.print("MQTT: ");
    while (!client.connected()) {
        if (client.connect("ESP32Client", mqtt_username, mqtt_password)) {
            Serial.println("Success");
            client.subscribe("device/ESP32/command");
        } else {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            delay(5000);
        }
    }
}

void MQTTClient::sendState(const char* message) {
    if (!client.publish("device/ESP32/response", message)) {
        Serial.println("Error sending state");
    }
}

void MQTTClient::sendLog(const char *message) {
    if (!client.publish("device/ESP32/log", message)) {
        Serial.println("Error sending log");
    }
}

void MQTTClient::loop() {
    if (!client.connected()) {
        connectMQTT();
    }
    client.loop();
}

void MQTTClient::callback(char* topic, const byte* payload, const unsigned int length) {
    String message;
    for (int i = 0; i < length; i++) {
        message += static_cast<char>(payload[i]);
    }
    if (instance->messageCallback) {
        instance->messageCallback(message);
    }
}

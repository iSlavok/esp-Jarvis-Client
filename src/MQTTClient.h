#ifndef MQTT_CLIENT_H
#define MQTT_CLIENT_H

#include <WiFi.h>
#include <PubSubClient.h>
#include <functional>  // Для std::function

class MQTTClient {
public:
    MQTTClient(const char* mqtt_server);


    void begin(std::function<void(const String&)> callback, const char* mqtt_username, const char* mqtt_password);
    void loop();
    void sendState(const char* message);
    void sendLog(const char* message);
private:
    const char* mqtt_server;
    const char* mqtt_username{};
    const char* mqtt_password{};
    WiFiClient espClient;
    PubSubClient client;
    std::function<void(const String&)> messageCallback;
    static MQTTClient* instance;

    void connectMQTT();
    static void callback(char* topic, const uint8_t* payload, unsigned int length);
};

#endif

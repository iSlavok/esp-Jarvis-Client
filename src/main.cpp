#include <WiFi.h>
#include "MQTTClient.h"
#include "Microphone.h"
#include "Audio/Audio.h"
#include "Led.h"
#include "Button.h"
#include "nvs_flash.h"
#include <cstring>
#include "WiFiManager.h"
#include "WebConfigServer.h"

const auto apSSID = "ESP32_Config";
const auto apPassword = "config123";
const auto mqttAddress = "103.97.88.123";
const auto udpAddress  = "103.97.88.123";
constexpr auto udpPort = 10052;
String host = "http://192.168.0.70:5252/audio-stream";
String state = "waiting";
const char* states[] = {"waiting", "recording", "responding", "speaking"};

#define I2S_WS_MIC    11
#define I2S_SD_MIC    13
#define I2S_SCK_MIC   14
#define I2S_DOUT_SPK  9
#define I2S_BCLK_SPK  3
#define I2S_LRC_SPK   8

void buttonCallback(int buttonState);

WiFiManager* wifiManager = nullptr;
WebConfigServer* webServer = nullptr;
MQTTClient mqttClient(mqttAddress);
Microphone mic(I2S_SCK_MIC, I2S_WS_MIC, I2S_SD_MIC, 22500, 128, 512, udpPort, udpAddress);
Audio audio;
Led led(5, 4);
Button button(19, buttonCallback);

vector<String> splitWString(const String& str, const char delimiter) {
    vector<String> words;
    int start = 0, end;
    while ((end = str.indexOf(delimiter, start)) != -1) {
        words.emplace_back(str.substring(start, end));
        start = end + 1;
    }
    words.emplace_back(str.substring(start));
    return words;
}

void updateState(const String& newState) {
    state = newState.c_str();
    if (state == "speaking") {
        audio.connecttohost(host.c_str());
        Serial.print("Host: ");
        Serial.println(host);
    }
    Serial.print("State updated to: ");
    Serial.println(state);
    mqttClient.sendLog(("State updated to: " + state).c_str());
}

void mqttCallback(const String& message) {
    auto parts = splitWString(message, ' ');
    if (parts[0] == "state") {
        for (const auto& _state : states) {
            if (strcmp(_state, parts[1].c_str()) == 0) {
                updateState(parts[1]);
            }
        }
    } else if (parts[0] == "volume") {
        audio.setVolume(parts[1].toInt());
        Serial.print("Volume updated to: ");
        Serial.println(parts[1]);
        mqttClient.sendLog(("Volume updated to: " + parts[1]).c_str());
    } else if (parts[0] == "host") {
        host = parts[1];
        Serial.print("Host updated to: ");
        Serial.println(host);
        mqttClient.sendLog(("Host updated to: " + parts[1]).c_str());
    } else {
        Serial.print("Invalid message: ");
        Serial.println(message);
        mqttClient.sendLog(("Invalid message: " + message).c_str());
    }
}

void buttonCallback(const int buttonState) {
    switch (buttonState) {
        case 0:
            if (state == "waiting") {
                state = "recording";
                mqttClient.sendLog("State updated to: recording");
                mqttClient.sendState("button_recording");
            } else if (state == "recording") {
                state = "responding";
                mqttClient.sendLog("State updated to: responding");
                mqttClient.sendState("responding");
            }
            break;
        case -1:
            if (state == "recording") {
                state = "responding";
                mqttClient.sendLog("State updated to: responding");
                mqttClient.sendState("responding");
            }
            break;
        default:
            break;
    }
}

String read_vars(const char* key) {
    nvs_handle_t my_handle;
    ESP_ERROR_CHECK(nvs_open("storage", NVS_READWRITE, &my_handle));
    size_t required_size;
    ESP_ERROR_CHECK(nvs_get_str(my_handle, key, nullptr, &required_size));
    const auto value = static_cast<char *>(malloc(required_size));
    ESP_ERROR_CHECK(nvs_get_str(my_handle, key, value, &required_size));
    auto result = String(value);
    free(value);
    nvs_close(my_handle);
    return result;
}

void initNVS() {
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
}

void connectWiFi() {
    wifiManager = new WiFiManager();
    wifiManager->begin();
    WiFi.softAP(apSSID, apPassword);
    Serial.print("AP IP: ");
    Serial.println(WiFi.softAPIP());
    webServer = new WebConfigServer(wifiManager);
    webServer->begin();
    Serial.print("WiFi: ");
    while (WiFiClass::status() != WL_CONNECTED) {
        led.wifiConnecting();
        Serial.print(".");
    }
    Serial.println(" Success");
}

void setup() {
    Serial.begin(115200);
    initNVS();
    led.begin();
    connectWiFi();
    mqttClient.begin(mqttCallback, read_vars("MQTT_USERNAME").c_str(), read_vars("MQTT_PASSWORD").c_str());
    mic.begin();
    button.begin();
    audio.setPinout(I2S_BCLK_SPK, I2S_LRC_SPK, I2S_DOUT_SPK);
    audio.setVolumeSteps(100);
    audio.setVolume(100);
    mqttClient.sendState("ready");
}

void loop() {
    led.loop(state);
    button.loop();
    mqttClient.loop();
    if (state == "waiting" || state == "recording") {
        mic.loop();
    } else if (state == "speaking") {
        audio.loop();
        if (audio.isEOF()) {
            Serial.println("Audio end");
            mqttClient.sendLog("Audio end");
            state = "waiting";
            mqttClient.sendLog("State updated to: waiting");
            mqttClient.sendState("waiting");
        }
    }
    vTaskDelay(1);
}

void audio_info(const char *info){
    Serial.println(info);
}
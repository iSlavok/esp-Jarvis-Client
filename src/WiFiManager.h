#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <WiFi.h>
#include <Preferences.h>
#include <vector>

struct WiFiCredential {
    String ssid;
    String password;
};

class WiFiManager {
public:
    WiFiManager();
    ~WiFiManager();
    void begin();
    std::vector<String> scanNetworks();
    bool connectToNetwork(const String &ssid, const String &password);
    bool autoConnect();
    bool addCredential(const String &ssid, const String &password);
    bool removeCredential(const String &ssid);
    std::vector<WiFiCredential> getSavedCredentials();

private:
    Preferences preferences;
    const char* PREF_NAMESPACE = "wifiCreds";
    const int MAX_NETWORKS = 5;

    void saveCredential(int index, const WiFiCredential &cred);
    bool loadCredential(int index, WiFiCredential &cred);
};

#endif

#include "WiFiManager.h"

WiFiManager::WiFiManager() {
    preferences.begin(PREF_NAMESPACE, false);
}

WiFiManager::~WiFiManager() {
    preferences.end();
}

void WiFiManager::begin() {
    WiFiClass::mode(WIFI_AP_STA);
    if(autoConnect()){
        Serial.println("Автоподключение выполнено успешно.");
    } else {
        Serial.println("Автоподключение не удалось.");
    }
}

std::vector<String> WiFiManager::scanNetworks() {
    std::vector<String> networks;
    int n = WiFi.scanNetworks();
    for (int i = 0; i < n; i++) {
        networks.push_back(WiFi.SSID(i));
    }
    return networks;
}

bool WiFiManager::connectToNetwork(const String &ssid, const String &password) {
    Serial.printf("Подключение к сети: %s\n", ssid.c_str());
    WiFi.begin(ssid.c_str(), password.c_str());
    int timeout = 30;
    while (WiFiClass::status() != WL_CONNECTED && timeout > 0) {
        delay(1000);
        Serial.print(".");
        timeout--;
    }
    if(WiFiClass::status() == WL_CONNECTED) {
        Serial.println("\nПодключено!");
        return true;
    }
    Serial.println("\nНе удалось подключиться.");
    return false;
}

bool WiFiManager::autoConnect() {
    std::vector<WiFiCredential> creds = getSavedCredentials();
    if(creds.empty()){
        Serial.println("Нет сохранённых сетей.");
        return false;
    }
    std::vector<String> available = scanNetworks();
    for(auto &cred : creds) {
        for(auto &net : available) {
            if(net == cred.ssid) {
                if(connectToNetwork(cred.ssid, cred.password)) {
                    return true;
                }
                Serial.printf("Не удалось подключиться к %s, пробую следующую\n", cred.ssid.c_str());
            }
        }
    }
    return false;
}

bool WiFiManager::addCredential(const String &ssid, const String &password) {
    // Если сеть уже сохранена – обновляем пароль
    std::vector<WiFiCredential> creds = getSavedCredentials();
    for(auto &cred : creds) {
        if(cred.ssid == ssid) {
            cred.password = password;
            for(int i = 0; i < MAX_NETWORKS; i++){
                WiFiCredential tmp;
                if(loadCredential(i, tmp)){
                    if(tmp.ssid == ssid){
                        saveCredential(i, cred);
                        return true;
                    }
                }
            }
        }
    }
    for(int i = 0; i < MAX_NETWORKS; i++){
        WiFiCredential tmp;
        if(!loadCredential(i, tmp) || tmp.ssid == ""){
            WiFiCredential newCred = {ssid, password};
            saveCredential(i, newCred);
            return true;
        }
    }
    const WiFiCredential newCred = {ssid, password};
    saveCredential(0, newCred);
    return true;
}

bool WiFiManager::removeCredential(const String &ssid) {
    for(int i = 0; i < MAX_NETWORKS; i++){
        WiFiCredential tmp;
        if(loadCredential(i, tmp)){
            if(tmp.ssid == ssid){
                preferences.remove(("ssid_" + String(i)).c_str());
                preferences.remove(("pass_" + String(i)).c_str());
                return true;
            }
        }
    }
    return false;
}

std::vector<WiFiCredential> WiFiManager::getSavedCredentials() {
    std::vector<WiFiCredential> creds;
    for(int i = 0; i < MAX_NETWORKS; i++){
        WiFiCredential cred;
        if(loadCredential(i, cred)){
            if(cred.ssid != "")
                creds.push_back(cred);
        }
    }
    return creds;
}

void WiFiManager::saveCredential(int index, const WiFiCredential &cred) {
    String ssidKey = "ssid_" + String(index);
    String passKey = "pass_" + String(index);
    preferences.putString(ssidKey.c_str(), cred.ssid);
    preferences.putString(passKey.c_str(), cred.password);
}

bool WiFiManager::loadCredential(int index, WiFiCredential &cred) {
    String ssidKey = "ssid_" + String(index);
    String passKey = "pass_" + String(index);
    String ssid = preferences.getString(ssidKey.c_str(), "");
    String pass = preferences.getString(passKey.c_str(), "");
    if(ssid == "")
        return false;
    cred.ssid = ssid;
    cred.password = pass;
    return true;
}

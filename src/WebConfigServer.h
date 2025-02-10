#ifndef WEB_CONFIG_SERVER_H
#define WEB_CONFIG_SERVER_H

#include <ESPAsyncWebServer.h>
#include "WiFiManager.h"

class WebConfigServer {
public:
    /// Конструктор принимает указатель на WiFiManager и порт (по умолчанию 80)
    WebConfigServer(WiFiManager* wifiManager, uint16_t port = 80);

    /// Запуск веб-сервера
    void begin();

private:
    AsyncWebServer server;
    WiFiManager* wifiManager;

    /// Формирование HTML-страницы для главного экрана
    String getHTMLPage();

    /// Формирование HTML-блока со списком доступных сетей
    String getNetworksHTML();
};

#endif // WEB_CONFIG_SERVER_H

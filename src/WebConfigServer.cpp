#include "WebConfigServer.h"

WebConfigServer::WebConfigServer(WiFiManager* wifiManager, uint16_t port)
        : server(port), wifiManager(wifiManager) { }

void WebConfigServer::begin() {
    server.on("/", HTTP_GET, [&](AsyncWebServerRequest *request){
        String html = getHTMLPage();
        request->send(200, "text/html", html);
    });
    server.on("/connect", HTTP_POST, [&](AsyncWebServerRequest *request){
        if(request->hasParam("ssid", true) && request->hasParam("password", true)){
            String ssid = request->getParam("ssid", true)->value();
            String password = request->getParam("password", true)->value();
            bool connected = wifiManager->connectToNetwork(ssid, password);
            if(connected){
                wifiManager->addCredential(ssid, password);
                request->send(200, "text/html",
                              "<html><body><h1>Подключено к сети " + ssid +
                              "</h1><a href='/'>Назад</a></body></html>");
            } else {
                request->send(200, "text/html",
                              "<html><body><h1>Не удалось подключиться к сети " + ssid +
                                "</h1><a href='/'>Назад</a></body></html>");
            }
        } else {
            request->send(400, "text/html", "Параметры не заданы");
        }
    });
    server.on("/autoconnect", HTTP_GET, [&](AsyncWebServerRequest *request){
        if(request->hasParam("ssid")){
            String ssid = request->getParam("ssid")->value();
            std::vector<WiFiCredential> saved = wifiManager->getSavedCredentials();
            String storedPassword = "";
            for(auto &cred : saved){
                if(cred.ssid == ssid){
                    storedPassword = cred.password;
                    break;
                }
            }
            if(storedPassword != ""){
                bool connected = wifiManager->connectToNetwork(ssid, storedPassword);
                if(connected){
                    request->send(200, "text/html",
                                  "<html><body><h1>Подключено к " + ssid + " с сохранённым паролем!</h1><a href='/'>Назад</a></body></html>");
                } else {
                    request->send(200, "text/html",
                                  "<html><body><h1>Не удалось подключиться к " + ssid +
                                  " с сохранённым паролем. Введите пароль вручную.</h1><a href='/'>Назад</a></body></html>");
                }
            } else {
                request->send(200, "text/html",
                              "<html><body><h1>Для сети " + ssid + " нет сохранённого пароля. Введите пароль вручную.</h1><a href='/'>Назад</a></body></html>");
            }
        } else {
            request->send(400, "text/html", "SSID не задан");
        }
    });

    // Эндпоинт для удаления сохранённой сети
    server.on("/remove", HTTP_POST, [&](AsyncWebServerRequest *request){
        if(request->hasParam("ssid", true)){
            String ssid = request->getParam("ssid", true)->value();
            bool removed = wifiManager->removeCredential(ssid);
            if(removed){
                request->send(200, "text/html",
                              "<html><body><h1>Сеть " + ssid + " удалена</h1><a href='/'>Назад</a></body></html>");
            } else {
                request->send(200, "text/html",
                              "<html><body><h1>Не удалось удалить сеть " + ssid + "</h1><a href='/'>Назад</a></body></html>");
            }
        } else {
            request->send(400, "text/html", "Параметры не заданы");
        }
    });

    server.begin();
}

String WebConfigServer::getNetworksHTML() {
    String html = "<h2>Доступные сети</h2>";
    // Сканирование сетей при заходе на сайт
    std::vector<String> networks = wifiManager->scanNetworks();
    // Получаем сохранённые данные для авто-подключения
    std::vector<WiFiCredential> saved = wifiManager->getSavedCredentials();

    html += "<ul>";
    for(auto &net : networks) {
        html += "<li>" + net;
        // Если для этой сети уже сохранён пароль – предлагаем автоподключение
        bool hasSaved = false;
        for(auto &cred : saved) {
            if(cred.ssid == net) {
                hasSaved = true;
                break;
            }
        }
        if(hasSaved) {
            html += " <a href='/autoconnect?ssid=" + net + "'>(Подключиться с сохранённым паролем)</a>";
        }
        html += "</li>";
    }
    html += "</ul>";
    return html;
}

String WebConfigServer::getHTMLPage() {
    String html = "<!DOCTYPE html><html><head><meta charset='UTF-8'><title>Настройка WiFi</title>";
    // Простой CSS для красивого вида страницы
    html += "<style>"
            "body { font-family: Arial, sans-serif; background-color: #f2f2f2; margin: 0; padding: 20px; }"
            ".container { max-width: 600px; margin: auto; background: #fff; padding: 20px; border-radius: 8px; }"
            "h1, h2 { text-align: center; }"
            "form { margin-top: 20px; }"
            "input[type='text'], input[type='password'] { width: 100%; padding: 10px; margin: 5px 0 10px 0; border: 1px solid #ccc; border-radius: 4px; }"
            "input[type='submit'] { background-color: #4CAF50; color: white; padding: 10px 20px; border: none; border-radius: 4px; cursor: pointer; }"
            "input[type='submit']:hover { background-color: #45a049; }"
            ".saved { background-color: #e7f3fe; padding: 10px; border: 1px solid #b3d7ff; border-radius: 4px; margin-top: 20px; }"
            "a { text-decoration: none; color: #0066cc; }"
            "</style></head><body>";

    html += "<div class='container'>";
    html += "<h1>Настройка WiFi</h1>";

    // Список доступных сетей
    html += getNetworksHTML();

    // Форма для ручного подключения
    html += "<h2>Подключиться к сети</h2>";
    html += "<form method='POST' action='/connect'>";
    html += "SSID:<br><input type='text' name='ssid' required><br>";
    html += "Пароль:<br><input type='password' name='password' required><br>";
    html += "<input type='submit' value='Подключиться'>";
    html += "</form>";

    // Список сохранённых сетей с возможностью удаления
    std::vector<WiFiCredential> saved = wifiManager->getSavedCredentials();
    if(!saved.empty()){
        html += "<div class='saved'><h2>Сохранённые сети</h2><ul>";
        for(auto &cred : saved){
            html += "<li>" + cred.ssid;
            html += " <form style='display:inline;' method='POST' action='/remove'>"
                    "<input type='hidden' name='ssid' value='" + cred.ssid + "'>"
                                                                             "<input type='submit' value='Удалить'>"
                                                                             "</form></li>";
        }
        html += "</ul></div>";
    }

    html += "</div></body></html>";
    return html;
}
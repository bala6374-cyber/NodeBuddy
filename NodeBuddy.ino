/**************************************************************
 *  NodeBuddy OS - Web Based OS for ESP8266
 *  Author: miniBotix / Kavinmayil
 *  Version: 1.0
 *  
 *  Features:
 *  - Web Desktop UI
 *  - WiFi Setup
 *  - File Manager (SPIFFS)
 *  - LED/GPIO Control
 *  - REST API
 *  - System Monitor
 *  - OTA Firmware Update
 **************************************************************/

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include <FS.h>
#include <ESP8266HTTPUpdateServer.h>

ESP8266WebServer server(80);
ESP8266HTTPUpdateServer updater;

#define LED_PIN 2     // NodeMCU onboard LED
bool ledState = false;

String wifi_ssid = "NodeBuddy-Setup";
String wifi_pass = "nodebuddy123";

/**************************************************************
 *   Utility — Serve SPIFFS Files
 **************************************************************/
bool serveFile(String path) {
    if (SPIFFS.exists(path)) {
        File file = SPIFFS.open(path, "r");
        server.streamFile(file, getContentType(path));
        file.close();
        return true;
    }
    return false;
}

String getContentType(String filename) {
    if (filename.endsWith(".html")) return "text/html";
    if (filename.endsWith(".css")) return "text/css";
    if (filename.endsWith(".js")) return "application/javascript";
    if (filename.endsWith(".json")) return "application/json";
    if (filename.endsWith(".png")) return "image/png";
    if (filename.endsWith(".jpg")) return "image/jpeg";
    return "text/plain";
}

/**************************************************************
 *   API: LED Control
 **************************************************************/
void apiLED() {
    if (server.hasArg("state")) {
        ledState = server.arg("state") == "1";
        digitalWrite(LED_PIN, !ledState);
    }
    StaticJsonDocument<128> doc;
    doc["led"] = ledState;
    String json;
    serializeJson(doc, json);
    server.send(200, "application/json", json);
}

/**************************************************************
 *   API: System Monitor
 **************************************************************/
void apiSystem() {
    StaticJsonDocument<256> doc;

    doc["heap"] = ESP.getFreeHeap();
    doc["chipid"] = ESP.getChipId();
    doc["wifi_rssi"] = WiFi.RSSI();
    doc["ip"] = WiFi.localIP().toString();
    doc["flash"] = ESP.getFlashChipRealSize();

    String json;
    serializeJson(doc, json);
    server.send(200, "application/json", json);
}

/**************************************************************
 *   API: SPIFFS Read File
 **************************************************************/
void apiReadFile() {
    if (!server.hasArg("f")) {
        server.send(400, "text/plain", "Missing filename");
        return;
    }

    String filename = server.arg("f");
    if (!SPIFFS.exists(filename)) {
        server.send(404, "text/plain", "File not found");
        return;
    }

    File file = SPIFFS.open(filename, "r");
    String data = file.readString();
    file.close();

    server.send(200, "text/plain", data);
}

/**************************************************************
 *   API: Write File
 **************************************************************/
void apiWriteFile() {
    if (!server.hasArg("f") || !server.hasArg("content")) {
        server.send(400, "text/plain", "Missing arguments");
        return;
    }

    String filename = server.arg("f");
    String content = server.arg("content");

    File file = SPIFFS.open(filename, "w");
    file.print(content);
    file.close();

    server.send(200, "text/plain", "Saved");
}

/**************************************************************
 *   API: List Files
 **************************************************************/
void apiListFiles() {
    StaticJsonDocument<1024> doc;
    JsonArray arr = doc.to<JsonArray>();

    Dir dir = SPIFFS.openDir("/");
    while (dir.next()) {
        JsonObject obj = arr.createNestedObject();
        obj["name"] = dir.fileName();
        obj["size"] = dir.fileSize();
    }

    String json;
    serializeJson(arr, json);
    server.send(200, "application/json", json);
}

/**************************************************************
 *   WiFi Setup Page
 **************************************************************/
void handleWiFiSetup() {
    if (server.hasArg("ssid") && server.hasArg("pass")) {
        wifi_ssid = server.arg("ssid");
        wifi_pass = server.arg("pass");

        server.send(200, "text/html",
            "<h3>Rebooting with new WiFi...</h3>");

        delay(1000);
        ESP.restart();
        return;
    }

    serveFile("/wifi.html");
}

/**************************************************************
 *   Router
 **************************************************************/
void setupServer() {
    server.on("/", []() {
        serveFile("/index.html");
    });

    server.on("/api/led", apiLED);
    server.on("/api/system", apiSystem);

    server.on("/api/file/read", apiReadFile);
    server.on("/api/file/write", apiWriteFile);
    server.on("/api/file/list", apiListFiles);

    server.on("/wifi", handleWiFiSetup);

    server.onNotFound([]() {
        if (!serveFile(server.uri())) {
            server.send(404, "text/plain", "Not Found");
        }
    });

    updater.setup(&server, "/update");

    server.begin();
}

/**************************************************************
 *   Main Setup
 **************************************************************/
void setup() {
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, HIGH);

    Serial.begin(115200);
    SPIFFS.begin();

    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP(wifi_ssid, wifi_pass);

    setupServer();
}

/**************************************************************
 *   Main Loop
 **************************************************************/
void loop() {
    server.handleClient();
}

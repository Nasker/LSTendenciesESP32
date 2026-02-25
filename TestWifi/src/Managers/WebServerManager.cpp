#include "Managers/WebServerManager.h"
#include "Managers/FileSystemManager.h"
#include "Managers/HardwareManager.h"
#include "Managers/WiFiManager.h"

WebServerManager::WebServerManager(FileSystemManager& fs, HardwareManager& hw, WiFiManager& wifi)
    : _server(80)
    , _fs(fs)
    , _hw(hw)
    , _wifi(wifi)
    , _running(false) {}

void WebServerManager::begin() {
    Serial.println("[WEB] Starting web server...");

    // Setup API routes
    _server.on("/", HTTP_GET, [this]() { handleRoot(); });
    _server.on("/api/led/toggle", HTTP_POST, [this]() { handleLedToggle(); });
    _server.on("/api/status", HTTP_GET, [this]() { handleStatus(); });
    _server.on("/api/system", HTTP_GET, [this]() { handleSystemInfo(); });
    
    // Handle static files and 404
    _server.onNotFound([this]() { handleNotFound(); });

    _server.begin();
    _running = true;
    
    Serial.println("[WEB] Server running");
}

void WebServerManager::handleClient() {
    if (_running) {
        _server.handleClient();
    }
}

void WebServerManager::handleRoot() {
    serveFile("/index.html", "text/html");
}

void WebServerManager::handleNotFound() {
    String path = _server.uri();
    
    // Try to serve as static file
    if (_fs.fileExists(path.c_str())) {
        serveFile(path.c_str(), getContentType(path).c_str());
    } else {
        _server.send(404, "text/plain", "404: Not Found");
    }
}

void WebServerManager::handleLedToggle() {
    bool newState = _hw.toggleLed();
    
    StaticJsonDocument<256> doc;
    doc["success"] = true;
    doc["ledState"] = newState;
    
    sendJson(doc);
    Serial.println("[API] LED toggled via web");
}

void WebServerManager::handleStatus() {
    StaticJsonDocument<256> doc;
    doc["ledState"] = _hw.getLedState();
    doc["buttonState"] = _hw.isButtonPressed();
    
    sendJson(doc);
}

void WebServerManager::handleSystemInfo() {
    StaticJsonDocument<512> doc;
    
    doc["ssid"] = _wifi.getCurrentSsid();
    doc["ip"] = _wifi.getLocalIp();
    doc["rssi"] = _wifi.getCurrentRssi();
    doc["uptime"] = _hw.getUptime();
    doc["mac"] = _wifi.getMacAddress();
    doc["hostname"] = WiFi.getHostname();
    
    sendJson(doc);
}

void WebServerManager::serveFile(const char* path, const char* contentType) {
    File file = _fs.openFile(path);
    if (!file) {
        _server.send(404, "text/plain", "File not found");
        return;
    }
    
    _server.streamFile(file, contentType);
    file.close();
}

String WebServerManager::getContentType(const String& path) const {
    if (path.endsWith(".html")) return "text/html";
    if (path.endsWith(".css")) return "text/css";
    if (path.endsWith(".js")) return "application/javascript";
    if (path.endsWith(".json")) return "application/json";
    if (path.endsWith(".png")) return "image/png";
    if (path.endsWith(".jpg")) return "image/jpeg";
    if (path.endsWith(".ico")) return "image/x-icon";
    return "text/plain";
}

void WebServerManager::sendJson(const JsonDocument& doc) {
    String response;
    serializeJson(doc, response);
    _server.send(200, "application/json", response);
}

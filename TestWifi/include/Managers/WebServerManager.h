#pragma once

#include <Arduino.h>
#include <WebServer.h>
#include <ArduinoJson.h>

// Forward declarations
class FileSystemManager;
class HardwareManager;
class WiFiManager;

/**
 * @brief Manages HTTP web server and REST API endpoints
 */
class WebServerManager {
public:
    WebServerManager(FileSystemManager& fs, HardwareManager& hw, WiFiManager& wifi);
    ~WebServerManager() = default;

    /**
     * @brief Start the web server
     */
    void begin();

    /**
     * @brief Handle client requests (call in loop)
     */
    void handleClient();

    /**
     * @brief Check if server is running
     */
    bool isRunning() const { return _running; }

private:
    WebServer _server;
    FileSystemManager& _fs;
    HardwareManager& _hw;
    WiFiManager& _wifi;
    bool _running;

    // Route handlers
    void handleRoot();
    void handleNotFound();
    void handleLedToggle();
    void handleStatus();
    void handleSystemInfo();
    void handleStaticFile();

    // Helper methods
    void serveFile(const char* path, const char* contentType);
    String getContentType(const String& path) const;
    void sendJson(const JsonDocument& doc);
};

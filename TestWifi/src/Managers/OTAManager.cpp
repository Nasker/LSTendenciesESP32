#include "Managers/OTAManager.h"

OTAManager::OTAManager(const char* hostname)
    : _hostname(hostname)
    , _password(nullptr)
    , _enabled(true)
    , _updating(false) {}

bool OTAManager::begin() {
    if (!_enabled) {
        return false;
    }

    // Set hostname if provided
    if (_hostname) {
        ArduinoOTA.setHostname(_hostname);
    }

    // Set password if provided
    if (_password) {
        ArduinoOTA.setPassword(_password);
    }

    setupCallbacks();

    ArduinoOTA.begin();
    
    Serial.println("[OTA] ArduinoOTA started");
    if (_hostname) {
        Serial.printf("[OTA] Hostname: %s\n", _hostname);
    }
    Serial.printf("[OTA] IP: %s\n", WiFi.localIP().toString().c_str());
    
    return true;
}

void OTAManager::handle() {
    if (_enabled) {
        ArduinoOTA.handle();
    }
}

void OTAManager::setPassword(const char* password) {
    _password = password;
}

void OTAManager::setupCallbacks() {
    ArduinoOTA.onStart([this]() {
        _updating = true;
        String type = (ArduinoOTA.getCommand() == U_FLASH) ? "sketch" : "filesystem";
        Serial.println("[OTA] Start updating " + type);
    });

    ArduinoOTA.onEnd([this]() {
        _updating = false;
        Serial.println("[OTA] Update complete");
        Serial.println("[OTA] Rebooting...");
    });

    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        static unsigned int lastPercent = 0;
        unsigned int percent = (progress * 100) / total;
        if (percent != lastPercent && percent % 10 == 0) {
            Serial.printf("[OTA] Progress: %u%%\n", percent);
            lastPercent = percent;
        }
    });

    ArduinoOTA.onError([this](ota_error_t error) {
        _updating = false;
        Serial.printf("[OTA] Error[%u]: ", error);
        
        switch (error) {
            case OTA_AUTH_ERROR:    Serial.println("Auth failed"); break;
            case OTA_BEGIN_ERROR:   Serial.println("Begin failed"); break;
            case OTA_CONNECT_ERROR: Serial.println("Connect failed"); break;
            case OTA_RECEIVE_ERROR: Serial.println("Receive failed"); break;
            case OTA_END_ERROR:     Serial.println("End failed"); break;
            default:                Serial.println("Unknown error");
        }
    });
}

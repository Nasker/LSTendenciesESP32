#pragma once

#include <Arduino.h>
#include <ArduinoOTA.h>

/**
 * @brief Manages ArduinoOTA for wireless firmware updates
 */
class OTAManager {
public:
    OTAManager(const char* hostname = nullptr);
    ~OTAManager() = default;

    /**
     * @brief Initialize ArduinoOTA
     * @return true if started successfully
     */
    bool begin();

    /**
     * @brief Handle OTA requests (call in loop)
     */
    void handle();

    /**
     * @brief Check if OTA is active (currently receiving update)
     */
    bool isUpdating() const { return _updating; }

    /**
     * @brief Set password for OTA updates
     */
    void setPassword(const char* password);

    /**
     * @brief Enable/disable OTA
     */
    void setEnabled(bool enabled) { _enabled = enabled; }

private:
    const char* _hostname;
    const char* _password;
    bool _enabled;
    bool _updating;

    void setupCallbacks();
};

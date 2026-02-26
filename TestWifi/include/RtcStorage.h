#pragma once

#include <Arduino.h>
#include <Preferences.h>

/**
 * @brief NVS storage for WiFi credentials across reboots
 * 
 * ESP32-C3 RTC memory is unreliable. We use NVS (flash) instead.
 * Workaround for ESP32-C3 Arduino WiFi bug: scan, store, reboot, connect.
 */
class NvsStorage {
public:
    // Call this after getting credentials from user to store them
    static void store(const String& ssid, const String& password);
    
    // Call this in setup() to check if we should auto-connect
    static bool shouldAutoConnect();
    
    // Retrieve stored credentials
    static void getCredentials(String& ssid, String& password);
    
    // Clear stored credentials
    static void clear();
};

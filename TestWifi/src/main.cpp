/*
#include <Arduino.h>
#include "AppController.h"

AppController app;

void setup() {
    app.begin();
}

void loop() {
    app.update();
}
*/

#include <Arduino.h>
#include <WiFi.h>

// ============================================
// MINIMAL CONNECTION TEST - EDIT THESE VALUES
// ============================================
const char* TEST_SSID = "NaskerAP";
const char* TEST_PASSWORD = "12345678";
// ============================================

void setup() {
    Serial.begin(115200);
    while (!Serial) { ; }
    delay(1000);
    
    Serial.println("\n========================================");
    Serial.println("    MINIMAL WIFI CONNECTION TEST");
    Serial.println("========================================");
    
    Serial.println("\n[TEST] Starting minimal connection test");
    Serial.println("[TEST] SSID: " + String(TEST_SSID));
    
    // Check if credentials are still default
    if (String(TEST_SSID) == "-") {
        Serial.println("\n[ERROR] Please edit TEST_SSID and TEST_PASSWORD in main.cpp!");
        Serial.println("[ERROR] Upload again after setting your credentials.");
        while (true) { delay(1000); }  // Halt
    }
    
    // Initialize WiFi
    WiFi.mode(WIFI_STA);
    Serial.println("[TEST] WiFi mode set to STA");
    
    // Disable power save - keeps radio fully powered
    WiFi.setTxPower(WIFI_POWER_19_5dBm);
    Serial.println("[TEST] TX power set to max");
    
    // Longer delay for RF calibration and radio startup
    delay(500);
    
    Serial.println("[TEST] MAC: " + WiFi.macAddress());
    Serial.println("[TEST] Initial status: " + String(WiFi.status()));
    
    // Start connection
    Serial.println("\n[TEST] Calling WiFi.begin()...");
    WiFi.begin(TEST_SSID, TEST_PASSWORD);
    
    // Wait for connection with status updates
    int attempts = 0;
    int lastStatus = -1;
    
    while (WiFi.status() != WL_CONNECTED && attempts < 30) {
        int status = WiFi.status();
        
        if (status != lastStatus) {
            Serial.println("[TEST] Status changed: " + String(status));
            lastStatus = status;
        } else {
            Serial.print(".");
        }
        
        delay(500);
        attempts++;
    }
    
    Serial.println("\n");
    
    // Result
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("[SUCCESS] Connected!");
        Serial.println("[SUCCESS] IP: " + WiFi.localIP().toString());
        Serial.println("[SUCCESS] RSSI: " + String(WiFi.RSSI()) + " dBm");
        
        // Keep connected and print heartbeat
        while (true) {
            delay(5000);
            Serial.println("[TEST] Still connected, IP: " + WiFi.localIP().toString());
        }
    } else {
        Serial.println("[FAILED] Connection failed!");
        Serial.println("[FAILED] Final status: " + String(WiFi.status()));
        Serial.println("\n[TEST] Retrying in 5 seconds...\n");
        delay(5000);
        ESP.restart();  // Reboot to retry
    }
}

void loop() {
    // Nothing here - everything in setup()
}

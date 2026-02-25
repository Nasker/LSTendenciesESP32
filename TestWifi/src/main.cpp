#include <Arduino.h>
#include "Managers/FileSystemManager.h"
#include "Managers/WiFiManager.h"
#include "Managers/HardwareManager.h"
#include "Managers/WebServerManager.h"
#include "Managers/OTAManager.h"

// Manager instances
FileSystemManager g_fs;
WiFiManager g_wifi;
HardwareManager g_hw;
WebServerManager g_web(g_fs, g_hw, g_wifi);
OTAManager g_ota;

// Forward declarations
void printBanner();
void printDiagnostics();
void promptAndConnect();
String readPasswordMasked();
void waitAndRescan();
void handleConnectedLoop();

// Global state
bool g_isConnected = false;
unsigned long g_lastStatusPrint = 0;

void setup() {
    Serial.begin(115200);
    while (!Serial) { ; }
    delay(1000);
    
    printBanner();
    
    // Initialize systems
    g_hw.begin();
    g_fs.begin();
    g_wifi.begin();
    
    printDiagnostics();
    
    // Set callback for when WiFi connects
    g_wifi.onConnected([]() {
        g_isConnected = true;
        g_web.begin();
        g_ota.begin();  // Start OTA updates
    });
}

void loop() {
    if (!g_isConnected) {
        promptAndConnect();
    } else {
        handleConnectedLoop();
    }
}

// -------------------------------------------------------------------------
// Helper Functions
// -------------------------------------------------------------------------

void printBanner() {
    Serial.println("\n========================================");
    Serial.println("    WiFi Network Scanner & Connector");
    Serial.println("========================================\n");
}

void printDiagnostics() {
    Serial.println("[DIAG] Device MAC Address: " + g_wifi.getMacAddress());
    Serial.println("[DIAG] WiFi Status: " + g_wifi.getStatusString());
    Serial.println();
}

void promptAndConnect() {
    // Scan for networks
    int count = g_wifi.scanNetworks();
    
    if (count == 0) {
        waitAndRescan();
        return;
    }
    
    // Prompt user
    Serial.println("\n-----------------------------------");
    Serial.println("Enter the number of the network to connect to (1-" + String(count) + "), or 0 to rescan:");
    
    // Wait for user input
    while (!Serial.available()) { delay(100); }
    
    int selection = Serial.parseInt();
    
    // Clear input buffer
    while (Serial.available()) { Serial.read(); }
    
    if (selection == 0) {
        Serial.println("[CONN] Rescanning...");
        WiFi.scanDelete();
        return;
    }
    
    if (selection < 1 || selection > count) {
        Serial.println("[CONN] Invalid selection.");
        return;
    }
    
    // Get network info
    int index = selection - 1;
    String ssid = g_wifi.getNetworkSsid(index);
    int rssi = g_wifi.getNetworkRssi(index);
    
    Serial.println("\n[CONN] Selected: " + ssid);
    Serial.println("[CONN] Signal: " + String(rssi) + " dBm");
    
    // Get password if needed
    String password = "";
    if (g_wifi.networkRequiresPassword(index)) {
        Serial.println("\n[CONN] Enter password (then press Enter):");
        password = readPasswordMasked();
        
        if (password.length() == 0) {
            Serial.println("[CONN] Password cannot be empty. Canceling.");
            return;
        }
        
        Serial.println("\n[CONN] Password length: " + String(password.length()) + " chars");
    } else {
        Serial.println("[CONN] Network is open, no password needed.");
    }
    
    // Attempt connection
    if (g_wifi.connect(ssid, password)) {
        g_isConnected = true;
        g_web.begin();
    } else {
        Serial.println("\n[CONN] Troubleshooting:");
        Serial.println("  - Double-check password (case sensitive)");
        Serial.println("  - Check if router uses MAC filtering");
        Serial.println("  - Try a different network");
        waitAndRescan();
    }
}

String readPasswordMasked() {
    String password = "";
    
    while (true) {
        if (Serial.available()) {
            char c = Serial.read();
            
            if (c == '\n' || c == '\r') {
                break;
            }
            
            if (c == '\b' || c == 127) {
                if (password.length() > 0) {
                    password.remove(password.length() - 1);
                    Serial.print("\b \b");
                }
                continue;
            }
            
            password += c;
            Serial.print('*');
        }
        delay(10);
    }
    
    return password;
}

void waitAndRescan() {
    Serial.println("\nPress any key to rescan immediately, or waiting 10 seconds...");
    
    int waitTime = 0;
    while (waitTime < 10000 && !Serial.available()) {
        delay(100);
        waitTime += 100;
    }
    
    while (Serial.available()) { Serial.read(); }
    
    WiFi.scanDelete();
}

void handleConnectedLoop() {
    // Handle OTA updates (must be called frequently)
    g_ota.handle();
    
    // Skip other tasks during OTA update
    if (g_ota.isUpdating()) {
        return;
    }
    
    // Handle web server requests
    g_web.handleClient();
    
    // Check for button changes
    g_hw.update();
    
    // Monitor connection
    if (!g_wifi.isConnected()) {
        Serial.println("\n⚠ Connection lost!");
        g_isConnected = false;
        return;
    }
    
    // Print status every 5 seconds
    if (millis() - g_lastStatusPrint > 5000) {
        g_lastStatusPrint = millis();
        Serial.println("Connected to " + g_wifi.getCurrentSsid() + 
                      " | IP: " + g_wifi.getLocalIp() + 
                      " | RSSI: " + String(g_wifi.getCurrentRssi()) + 
                      " dBm | Server Active");
    }
}
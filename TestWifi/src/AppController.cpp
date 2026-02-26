#include "AppController.h"
#include "RtcStorage.h"
#include <esp_system.h>

AppController::AppController()
    : _web(_fs, _hw, _wifi),
      _connected(false),
      _lastStatusPrint(0)
{}

void AppController::begin() {
    Serial.begin(115200);
    while (!Serial) { ; }
    delay(1000);

    printBanner();
    
    // Check reset reason
    esp_reset_reason_t reason = esp_reset_reason();
    Serial.println("[BOOT] Reset reason: " + String(reason));
    if (reason == ESP_RST_SW) {
        Serial.println("[BOOT] Software reset - NVS should have credentials");
    } else if (reason == ESP_RST_POWERON) {
        Serial.println("[BOOT] Power-on reset - NVS may have credentials");
    }

    _hw.begin();
    _fs.begin();
    _wifi.begin();

    // Try auto-connect with stored credentials if network is available
    if (_wifi.tryAutoConnect()) {
        _connected = true;
        _web.begin();
    }

    printDiagnostics();
    _ota.begin();
}

void AppController::update() {
    if (!_connected) {
        promptAndConnect();
    } else {
        handleConnectedLoop();
    }
}

// -------------------------------------------------------------------------
// Setup helpers
// -------------------------------------------------------------------------

void AppController::printBanner() {
    Serial.println("\n========================================");
    Serial.println("    WiFi Network Scanner & Connector");
    Serial.println("========================================\n");
}

void AppController::printDiagnostics() {
    Serial.println("[DIAG] Device MAC Address: " + _wifi.getMacAddress());
    Serial.println("[DIAG] WiFi Status: " + _wifi.getStatusString());
    Serial.println();
}

// -------------------------------------------------------------------------
// Disconnected flow
// -------------------------------------------------------------------------

void AppController::promptAndConnect() {
    int count = _wifi.scanNetworks();

    if (count == 0) {
        waitAndRescan();
        return;
    }

    Serial.println("\n-----------------------------------");
    Serial.println("Enter the number of the network to connect to (1-" + String(count) + "), or 0 to rescan:");

    while (!Serial.available()) { delay(100); }

    int selection = Serial.parseInt();
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

    int index = selection - 1;
    String ssid = _wifi.getNetworkSsid(index);
    int rssi = _wifi.getNetworkRssi(index);

    Serial.println("\n[CONN] Selected: " + ssid);
    Serial.println("[CONN] Signal: " + String(rssi) + " dBm");

    String password = "";
    if (_wifi.networkRequiresPassword(index)) {
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

    // Connect directly - TX power fix handles the interference issue
    Serial.println("\n[CONN] Connecting...");
    
    if (_wifi.connect(ssid, password)) {
        _connected = true;
        _web.begin();
        
        // Store credentials for auto-connect next time
        NvsStorage::store(ssid, password);
        Serial.println("[CONN] Credentials stored for auto-connect");
        
        Serial.println("\n[CONN] Connected successfully!");
    } else {
        Serial.println("\n[CONN] Connection failed. Check password and try again.");
        waitAndRescan();
    }
}

String AppController::readPasswordMasked() {
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

void AppController::waitAndRescan() {
    Serial.println("\nPress any key to rescan immediately, or waiting 10 seconds...");

    int waitTime = 0;
    while (waitTime < 10000 && !Serial.available()) {
        delay(100);
        waitTime += 100;
    }

    while (Serial.available()) { Serial.read(); }
    WiFi.scanDelete();
}

// -------------------------------------------------------------------------
// Connected flow
// -------------------------------------------------------------------------

void AppController::handleConnectedLoop() {
    _ota.handle();

    if (_ota.isUpdating()) {
        return;
    }

    _web.handleClient();
    _hw.update();

    if (!_wifi.isConnected()) {
        Serial.println("\n⚠ Connection lost!");
        _connected = false;
        return;
    }

    if (millis() - _lastStatusPrint > 5000) {
        _lastStatusPrint = millis();
        Serial.println("Connected to " + _wifi.getCurrentSsid() +
                       " | IP: " + _wifi.getLocalIp() +
                       " | RSSI: " + String(_wifi.getCurrentRssi()) +
                       " dBm | Server Active");
    }
}

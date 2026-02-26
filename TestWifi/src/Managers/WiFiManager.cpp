#include "Managers/WiFiManager.h"
#include "RtcStorage.h"

WiFiManager::WiFiManager() : _networkCount(0), _onConnected(nullptr) {}

void WiFiManager::begin() {
    // Initialize WiFi to get TCP/IP stack ready
    WiFi.mode(WIFI_STA);
    WiFi.persistent(true);  // Don't save credentials to flash
    WiFi.setAutoReconnect(true);  // We'll manage connection manually
    
    // ESP32-C3 board layout interference fix: reduce TX power
    WiFi.setTxPower(WIFI_POWER_8_5dBm);
    Serial.println("[WIFI] TX power reduced to 8.5dBm for board stability");
}

int WiFiManager::scanNetworks() {
    Serial.println("[SCAN] Starting async WiFi scan...");
    
    // Start async scan (true = async, false = blocking)
    WiFi.scanNetworks(true);
    
    // Poll for completion
    int scanStatus;
    while ((scanStatus = WiFi.scanComplete()) == WIFI_SCAN_RUNNING) {
        delay(100);
        Serial.print(".");
    }
    
    Serial.println();
    
    // scanStatus now contains result count or error
    if (scanStatus == WIFI_SCAN_FAILED) {
        _networkCount = -1;
        Serial.println("[SCAN] Scan failed with error: WIFI_SCAN_FAILED");
    } else if (scanStatus == 0) {
        _networkCount = 0;
        Serial.println("[SCAN] No networks found");
    } else if (scanStatus > 0) {
        _networkCount = scanStatus;
        Serial.print("[SCAN] ");
        Serial.print(_networkCount);
        Serial.println(" networks found:");
        printScanResults();
    } else {
        _networkCount = scanStatus;  // Should not happen, but handle it
        Serial.println("[SCAN] Unexpected scan result: " + String(scanStatus));
    }
    
    return _networkCount;
}

void WiFiManager::printScanResults() const {
    if (_networkCount == 0) return;
    
    Serial.printf("%-4s %-30s %-10s %-15s\n", "No.", "SSID", "RSSI", "Encryption");
    Serial.println("--------------------------------------------------------------");
    
    for (int i = 0; i < _networkCount; i++) {
        Serial.printf("%-4d %-30s %-10d %-15s\n", 
                      i + 1, 
                      WiFi.SSID(i).c_str(), 
                      WiFi.RSSI(i), 
                      getNetworkEncryption(i).c_str());
    }
    Serial.println();
}

String WiFiManager::getNetworkSsid(int index) const {
    if (index >= 0 && index < _networkCount) {
        return WiFi.SSID(index);
    }
    return "";
}

int WiFiManager::getNetworkRssi(int index) const {
    if (index >= 0 && index < _networkCount) {
        return WiFi.RSSI(index);
    }
    return 0;
}

String WiFiManager::getNetworkEncryption(int index) const {
    if (index >= 0 && index < _networkCount) {
        return getEncryptionTypeString(WiFi.encryptionType(index));
    }
    return "Unknown";
}

bool WiFiManager::networkRequiresPassword(int index) const {
    if (index >= 0 && index < _networkCount) {
        return WiFi.encryptionType(index) != WIFI_AUTH_OPEN;
    }
    return false;
}

bool WiFiManager::waitForIdle(unsigned long timeoutMs) {
    unsigned long start = millis();
    
    WiFi.scanDelete();
    WiFi.disconnect(true, false);
    WiFi.mode(WIFI_OFF);
    delay(200);
    WiFi.mode(WIFI_STA);
    
    while (WiFi.status() != WL_IDLE_STATUS) {
        if (millis() - start > timeoutMs) {
            Serial.println("[CONN] Could not reach IDLE (at " + wifiStatusToString(WiFi.status()) + ")");
            return false;
        }
        delay(100);
        Serial.print(".");
    }
    
    Serial.println("\n[CONN] WiFi IDLE reached (" + String(millis() - start) + "ms)");
    return true;
}

bool WiFiManager::connect(const String& ssid, const String& password) {
    Serial.println("\n[CONN] Connecting to: " + ssid);
    
    // Simple connection - TX power fix is already applied in begin()
    WiFi.begin(ssid.c_str(), password.c_str());
    
    int attempts = 0;
    int lastStatus = -1;
    
    while (!isConnected() && attempts < 60) {
        int currentStatus = WiFi.status();
        
        if (currentStatus != lastStatus) {
            Serial.println("[CONN] Status: " + wifiStatusToString(currentStatus));
            lastStatus = currentStatus;
        } else {
            Serial.print(".");
        }
        
        delay(500);
        attempts++;
        
        if (attempts % 10 == 0 && currentStatus == lastStatus) {
            Serial.println(" (" + String(attempts * 0.5) + "s)");
        }
    }
    
    Serial.println("\n[CONN] Final: " + getStatusString());
    
    if (isConnected()) {
        Serial.println("✓ Connected! IP: " + getLocalIp());
        return true;
    }
    
    Serial.println("✗ Failed");
    return false;
}

bool WiFiManager::tryAutoConnect() {
    // Check if we have stored credentials
    if (!NvsStorage::shouldAutoConnect()) {
        return false;
    }
    
    String storedSsid, storedPassword;
    NvsStorage::getCredentials(storedSsid, storedPassword);
    
    if (storedSsid.length() == 0) {
        return false;
    }
    
    Serial.println("\n[AUTO] Stored credentials found for: " + storedSsid);
    Serial.println("[AUTO] Scanning for matching network...");
    
    // Scan networks
    int count = scanNetworks();
    if (count <= 0) {
        Serial.println("[AUTO] No networks found, auto-connect failed");
        return false;
    }
    
    // Check if stored SSID is in scan results
    bool found = false;
    int foundIndex = -1;
    for (int i = 0; i < count; i++) {
        if (WiFi.SSID(i) == storedSsid) {
            found = true;
            foundIndex = i;
            break;
        }
    }
    
    if (!found) {
        Serial.println("[AUTO] Stored network '" + storedSsid + "' not found in scan");
        return false;
    }
    
    Serial.println("[AUTO] Network found! RSSI: " + String(WiFi.RSSI(foundIndex)) + " dBm");
    Serial.println("[AUTO] Connecting...");
    
    // Clear credentials so we don't auto-connect again on next boot
    NvsStorage::clear();
    
    // Connect
    return connect(storedSsid, storedPassword);
}

void WiFiManager::disconnect() {
    WiFi.disconnect(true, true);
}

bool WiFiManager::isConnected() const {
    return WiFi.status() == WL_CONNECTED;
}

String WiFiManager::getStatusString() const {
    return wifiStatusToString(WiFi.status());
}

String WiFiManager::getLocalIp() const {
    return WiFi.localIP().toString();
}

String WiFiManager::getCurrentSsid() const {
    return WiFi.SSID();
}

int WiFiManager::getCurrentRssi() const {
    return WiFi.RSSI();
}

String WiFiManager::getMacAddress() const {
    return WiFi.macAddress();
}

String WiFiManager::getEncryptionTypeString(int encryptionType) const {
    switch (encryptionType) {
        case WIFI_AUTH_OPEN:           return "Open";
        case WIFI_AUTH_WEP:            return "WEP";
        case WIFI_AUTH_WPA_PSK:        return "WPA";
        case WIFI_AUTH_WPA2_PSK:       return "WPA2";
        case WIFI_AUTH_WPA_WPA2_PSK:   return "WPA/WPA2";
        case WIFI_AUTH_WPA3_PSK:       return "WPA3";
        default:                       return "Unknown";
    }
}

String WiFiManager::wifiStatusToString(int status) const {
    switch (status) {
        case WL_IDLE_STATUS:     return "IDLE (0)";
        case WL_NO_SSID_AVAIL:   return "NO_SSID_AVAIL (1)";
        case WL_SCAN_COMPLETED:  return "SCAN_COMPLETED (2)";
        case WL_CONNECTED:       return "CONNECTED (3)";
        case WL_CONNECT_FAILED:  return "CONNECT_FAILED (4)";
        case WL_CONNECTION_LOST: return "CONNECTION_LOST (5)";
        case WL_DISCONNECTED:    return "DISCONNECTED (6)";
        default:                 return "UNKNOWN (" + String(status) + ")";
    }
}

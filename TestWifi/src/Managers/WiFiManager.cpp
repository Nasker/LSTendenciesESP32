#include "Managers/WiFiManager.h"

WiFiManager::WiFiManager() : _networkCount(0), _onConnected(nullptr) {}

void WiFiManager::begin() {
    WiFi.mode(WIFI_STA);
}

int WiFiManager::scanNetworks() {
    Serial.println("[SCAN] Scanning for WiFi networks...\n");
    _networkCount = WiFi.scanNetworks();
    
    if (_networkCount == 0) {
        Serial.println("[SCAN] No networks found");
    } else if (_networkCount < 0) {
        Serial.println("[SCAN] Scan failed with error: " + String(_networkCount));
    } else {
        Serial.print("[SCAN] ");
        Serial.print(_networkCount);
        Serial.println(" networks found:\n");
        printScanResults();
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

bool WiFiManager::connect(const String& ssid, const String& password) {
    Serial.println("\n[CONN] Starting connection to: " + ssid);
    Serial.println("[CONN] Initial status: " + getStatusString());
    
    disconnect();
    delay(100);
    
    WiFi.begin(ssid.c_str(), password.c_str());
    
    int attempts = 0;
    int lastStatus = -1;
    
    while (!isConnected() && attempts < 60) {
        int currentStatus = WiFi.status();
        
        if (currentStatus != lastStatus) {
            Serial.println("\n[CONN] Status changed: " + wifiStatusToString(currentStatus));
            lastStatus = currentStatus;
        } else {
            Serial.print(".");
        }
        
        delay(500);
        attempts++;
        
        if (attempts % 10 == 0 && currentStatus == lastStatus) {
            Serial.println(" (" + String(attempts * 0.5) + "s elapsed)");
        }
    }
    
    Serial.println("\n[CONN] Final status: " + getStatusString());
    
    if (isConnected()) {
        Serial.println("\n✓ [CONN] Connected successfully!");
        Serial.println("[CONN] IP address: " + getLocalIp());
        Serial.println("[CONN] Signal strength: " + String(getCurrentRssi()) + " dBm");
        
        if (_onConnected) {
            _onConnected();
        }
        return true;
    }
    
    Serial.println("\n✗ [CONN] Failed to connect!");
    return false;
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

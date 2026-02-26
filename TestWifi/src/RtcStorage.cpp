#include "RtcStorage.h"

static const char* NVS_NAMESPACE = "wificonn";
static const char* KEY_SSID = "ssid";
static const char* KEY_PASS = "pass";
static const char* KEY_VALID = "valid";

void NvsStorage::store(const String& ssid, const String& password) {
    Serial.println("[NVS] Storing credentials for: " + ssid);
    
    Preferences prefs;
    prefs.begin(NVS_NAMESPACE, false);  // Read-write mode
    prefs.putString(KEY_SSID, ssid);
    prefs.putString(KEY_PASS, password);
    prefs.putBool(KEY_VALID, true);
    prefs.end();
    
    Serial.println("[NVS] Stored successfully");
}

bool NvsStorage::shouldAutoConnect() {
    Preferences prefs;
    prefs.begin(NVS_NAMESPACE, true);  // Read-only mode
    bool valid = prefs.getBool(KEY_VALID, false);
    prefs.end();
    
    Serial.println("[NVS] Checking - valid=" + String(valid ? "true" : "false"));
    return valid;
}

void NvsStorage::getCredentials(String& ssid, String& password) {
    Preferences prefs;
    prefs.begin(NVS_NAMESPACE, true);  // Read-only mode
    ssid = prefs.getString(KEY_SSID, "");
    password = prefs.getString(KEY_PASS, "");
    prefs.end();
    
    Serial.println("[NVS] Retrieved SSID: " + ssid);
}

void NvsStorage::clear() {
    Serial.println("[NVS] Clearing stored credentials");
    
    Preferences prefs;
    prefs.begin(NVS_NAMESPACE, false);  // Read-write mode
    prefs.clear();
    prefs.end();
}

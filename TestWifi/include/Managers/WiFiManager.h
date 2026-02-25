#pragma once

#include <Arduino.h>
#include <WiFi.h>

/**
 * @brief Manages WiFi connection and network scanning
 */
class WiFiManager {
public:
    using ConnectionCallback = void (*)(void);

    WiFiManager();
    ~WiFiManager() = default;

    /**
     * @brief Initialize WiFi in station mode
     */
    void begin();

    /**
     * @brief Scan for available networks
     * @return Number of networks found
     */
    int scanNetworks();

    /**
     * @brief Get number of networks from last scan
     */
    int getNetworkCount() const { return _networkCount; }

    /**
     * @brief Print scan results to Serial
     */
    void printScanResults() const;

    /**
     * @brief Get SSID of scanned network
     */
    String getNetworkSsid(int index) const;

    /**
     * @brief Get RSSI of scanned network
     */
    int getNetworkRssi(int index) const;

    /**
     * @brief Get encryption type string of scanned network
     */
    String getNetworkEncryption(int index) const;

    /**
     * @brief Check if network requires password
     */
    bool networkRequiresPassword(int index) const;

    /**
     * @brief Connect to a network with password
     * @param ssid Network name
     * @param password Password (empty for open networks)
     * @return true if connected successfully
     */
    bool connect(const String& ssid, const String& password);

    /**
     * @brief Disconnect from WiFi
     */
    void disconnect();

    /**
     * @brief Check if connected to WiFi
     */
    bool isConnected() const;

    /**
     * @brief Get current WiFi status string
     */
    String getStatusString() const;

    /**
     * @brief Get current IP address as string
     */
    String getLocalIp() const;

    /**
     * @brief Get current SSID
     */
    String getCurrentSsid() const;

    /**
     * @brief Get current RSSI
     */
    int getCurrentRssi() const;

    /**
     * @brief Get MAC address
     */
    String getMacAddress() const;

    /**
     * @brief Set callback for when connection is established
     */
    void onConnected(ConnectionCallback callback) { _onConnected = callback; }

private:
    int _networkCount;
    ConnectionCallback _onConnected;

    String getEncryptionTypeString(int encryptionType) const;
    String wifiStatusToString(int status) const;
};

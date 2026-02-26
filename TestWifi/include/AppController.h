#pragma once

#include <Arduino.h>
#include "Managers/FileSystemManager.h"
#include "Managers/WiFiManager.h"
#include "Managers/HardwareManager.h"
#include "Managers/WebServerManager.h"
#include "Managers/OTAManager.h"

/**
 * @brief Top-level application controller that orchestrates all managers
 *        and handles the serial UI flow.
 */
class AppController {
public:
    AppController();
    ~AppController() = default;

    /**
     * @brief Initialize all subsystems (call from setup())
     */
    void begin();

    /**
     * @brief Run one iteration of the main loop (call from loop())
     */
    void update();

private:
    // Managers
    FileSystemManager _fs;
    WiFiManager       _wifi;
    HardwareManager   _hw;
    WebServerManager  _web;
    OTAManager        _ota;

    // State
    bool          _connected;
    unsigned long _lastStatusPrint;

    // Setup helpers
    void printBanner();
    void printDiagnostics();

    // Disconnected flow
    void promptAndConnect();
    String readPasswordMasked();
    void waitAndRescan();

    // Connected flow
    void handleConnectedLoop();
};

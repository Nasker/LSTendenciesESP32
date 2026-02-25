#pragma once

#include <Arduino.h>

/**
 * @brief Manages hardware I/O (LED and button)
 */
class HardwareManager {
public:
    using ButtonCallback = void (*)(bool pressed);

    HardwareManager(uint8_t ledPin = 8, uint8_t buttonPin = 9);
    ~HardwareManager() = default;

    /**
     * @brief Initialize hardware pins
     */
    void begin();

    /**
     * @brief Set LED state
     */
    void setLed(bool on);

    /**
     * @brief Toggle LED state
     * @return New LED state
     */
    bool toggleLed();

    /**
     * @brief Get current LED state
     */
    bool getLedState() const { return _ledState; }

    /**
     * @brief Read current button state
     * @return true if pressed, false if released
     */
    bool isButtonPressed() const;

    /**
     * @brief Check for button state change (call in loop)
     * @return true if state changed since last check
     */
    bool update();

    /**
     * @brief Set callback for button state changes
     */
    void onButtonChange(ButtonCallback callback) { _buttonCallback = callback; }

    /**
     * @brief Get elapsed milliseconds since boot
     */
    unsigned long getUptime() const { return (millis() - _bootTime) / 1000; }

private:
    uint8_t _ledPin;
    uint8_t _buttonPin;
    bool _ledState;
    bool _lastButtonState;
    unsigned long _lastButtonCheck;
    unsigned long _bootTime;
    ButtonCallback _buttonCallback;

    static constexpr unsigned long BUTTON_DEBOUNCE_MS = 50;
};

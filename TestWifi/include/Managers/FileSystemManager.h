#pragma once

#include <Arduino.h>
#include <LittleFS.h>

/**
 * @brief Manages LittleFS filesystem operations
 */
class FileSystemManager {
public:
    FileSystemManager();
    ~FileSystemManager() = default;

    /**
     * @brief Initialize LittleFS filesystem
     * @return true if mounted successfully
     */
    bool begin();

    /**
     * @brief Check if filesystem is mounted
     */
    bool isMounted() const { return _mounted; }

    /**
     * @brief List all files in root directory to Serial
     */
    void listFiles() const;

    /**
     * @brief Check if file exists
     */
    bool fileExists(const char* path) const;

    /**
     * @brief Open a file for reading
     * @return File object (check with file.isValid())
     */
    File openFile(const char* path) const;

private:
    bool _mounted;
};

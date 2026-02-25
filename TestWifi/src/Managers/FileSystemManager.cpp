#include "Managers/FileSystemManager.h"

FileSystemManager::FileSystemManager() : _mounted(false) {}

bool FileSystemManager::begin() {
    Serial.println("[FS] Initializing LittleFS...");
    
    if (!LittleFS.begin(true)) {
        Serial.println("[FS] LittleFS Mount Failed");
        _mounted = false;
        return false;
    }
    
    _mounted = true;
    Serial.println("[FS] LittleFS mounted successfully");
    
    listFiles();
    return true;
}

void FileSystemManager::listFiles() const {
    if (!_mounted) {
        Serial.println("[FS] Filesystem not mounted");
        return;
    }
    
    File root = LittleFS.open("/");
    if (!root || !root.isDirectory()) {
        Serial.println("[FS] Failed to open root directory");
        return;
    }
    
    Serial.println("[FS] Files in /:");
    File file = root.openNextFile();
    int fileCount = 0;
    
    while (file) {
        if (!file.isDirectory()) {
            Serial.printf("  /%s (%d bytes)\n", file.name(), file.size());
            fileCount++;
        }
        file = root.openNextFile();
    }
    
    if (fileCount == 0) {
        Serial.println("[FS] WARNING: No files found! Run 'Upload Filesystem Image' in PlatformIO");
    }
}

bool FileSystemManager::fileExists(const char* path) const {
    if (!_mounted) return false;
    return LittleFS.exists(path);
}

File FileSystemManager::openFile(const char* path) const {
    if (!_mounted) return File();
    return LittleFS.open(path, "r");
}

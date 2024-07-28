#include "ProgressStore.hpp"

ProgressStore::ProgressStore() {
    if (!SPIFFS.begin(true)) {
        Serial.println("An Error has occurred while mounting SPIFFS");
        return;
    }
    progress.value = 0;
    progress.percentage = 0;
    progress.message = "";
    loadProgress();
}

void ProgressStore::incrementProgress() {
    progress.value++;
    saveProgress();
}

void ProgressStore::decrementProgress() {
    progress.value--;
    saveProgress();
}

void ProgressStore::setProgress(int newProgress) {
    progress.value = newProgress;
    saveProgress();
}

void ProgressStore::resetProgress() {
    progress.value = 0;
    saveProgress();
}

ProgressData ProgressStore::getProgress() {
    progress.percentage = (progress.value * 100) / MAX_PROGRESS;
    if (progress.value == MAX_PROGRESS) {
        progress.message = "Felicitats Unai! Ho has aconseguit!";
    } else {
        progress.message = "Vinga som-hi! Cada dia estàs més a prop!";
    }
    return progress;
}

void ProgressStore::saveProgress() {
    JsonDocument doc;
    doc["value"] = progress.value;

    File file = SPIFFS.open(PROG_FILE, FILE_WRITE);
    if (!file) {
        Serial.println("Failed to open progress file for writing");
        return;
    }
    serializeJson(doc, file);
    file.close();
}

void ProgressStore::loadProgress() {
    File file = SPIFFS.open(PROG_FILE, FILE_READ);
    if (!file) {
        Serial.println("Failed to open progress file for reading");
        return;
    }

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, file);
    if (error) {
        Serial.println("Failed to read file, using default progress");
        return;
    }

    progress.value = doc["value"] | 0;
    file.close();
}

#pragma once

#include <SPIFFS.h>
#include <ArduinoJson.h>

struct ProgressData {
  int value;
  int percentage;
  String message;
};

#define PROG_FILE "/settings/progress.json"
#define MAX_PROGRESS 30

class ProgressStore {
  public:
    ProgressStore();
    void incrementProgress();
    void decrementProgress();
    void setProgress(int progress);
    void resetProgress();
    ProgressData getProgress();
    void saveProgress();
    void loadProgress();
  private:
    ProgressData progress;
};
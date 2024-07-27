#pragma once

#include <EEPROM.h>

#define PROGRESS_ADDRESS 0
const int MAX_PROGRESS = 30;

class ProgressStore {
  public:
    ProgressStore();
    void incrementProgress();
    void decrementProgress();
    void setProgress(int progress);
    void resetProgress();
    int getProgress();
    void saveProgress();
    void loadProgress();
  private:
    int progress;
};
#pragma once

#include <EEPROM.h>

#define PROGRESS_ADDRESS 0

class ProgressStore {
  public:
    ProgressStore();
    void incrementProgress();
    void resetProgress();
    int getProgress();
    void saveProgress();
    void loadProgress();
  private:
    int progress;
};
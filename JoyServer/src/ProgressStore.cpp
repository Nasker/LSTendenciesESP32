#include "ProgressStore.hpp"

ProgressStore::ProgressStore(){
    progress = 0;
    loadProgress();
    EEPROM.begin(1);
}

void ProgressStore::incrementProgress(){
    progress++;
}

void ProgressStore::decrementProgress(){
    progress--;
}

void ProgressStore::setProgress(int newProgress){
    progress = newProgress;
}

void ProgressStore::resetProgress(){
    progress--;
}

int ProgressStore::getProgress(){
    return progress;
}

void ProgressStore::saveProgress(){
    EEPROM.write(PROGRESS_ADDRESS, progress);
    EEPROM.commit();
}

void ProgressStore::loadProgress(){
    progress = EEPROM.read(PROGRESS_ADDRESS);
}
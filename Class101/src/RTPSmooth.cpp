/*
	RTPSmooth.cpp - Class for smoothing analog readings.
	Created by Oscar Martínez Carmona @ RockinTechProjects, February 3, 2016.
*/

#include "Arduino.h"
#include "RTPSmooth.h"


RTPSmooth::RTPSmooth(){
	_readIndex = 0;              
	_total = 0;                  
	_average = 0;  
	
	for (int thisReading = 0; thisReading < _numReadings; thisReading++)
    	_readings[thisReading] = 0;
}


int  RTPSmooth::smooth(int inputValue){
  _total = _total - _readings[_readIndex];
  // read from the sensor:
  _readings[_readIndex] = inputValue;
  // add the reading to the total:
  _total += _readings[_readIndex];
  // advance to the next position in the array:
  _readIndex++;

  // if we're at the end of the array...
  if (_readIndex >= _numReadings) _readIndex = 0;

  // calculate the average:
  _average = _total / _numReadings;

  return _average;
 }




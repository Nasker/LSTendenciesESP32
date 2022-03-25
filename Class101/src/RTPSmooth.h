/*
	RTPSmooth.h - Class for smoothing analog readings.
	Created by Oscar Martínez Carmona @ RockinTechProjects, February 3, 2016.
*/

#ifndef RTPSmooth_h
#define RTPSmooth_h

#include "Arduino.h"

const int _numReadings = 20;

class RTPSmooth{
int _readings[_numReadings]; 
int _readIndex;              
int _total;                
int _average;   
	
	public:
		RTPSmooth();
		int smooth(int inputValue);
};

#endif

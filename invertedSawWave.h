
#ifndef _INVERTEDSAWWAVE_H_
#define _INVERTEDSAWWAVE_H_
#include "oscillator.h"
#include <iostream>

class invertedSawWave : public Oscillator
{
public:
    
    //constructors and destructor
    invertedSawWave(double samplerate): invertedSawWave(samplerate, 0, 0) {};
    invertedSawWave(double samplerate, double frequency) :
    invertedSawWave(samplerate, frequency, 0) {};
    invertedSawWave(double samplerate, double frequency, double phase):
    Oscillator (samplerate, frequency, phase) {};
    ~invertedSawWave(){};
    //prevent the default constructor to be generated
    invertedSawWave() = delete;
    
    //override calculate
    //this method contains the sample calculation
    void calculate() override;
    
    
};


//override calculate method
//this method contains the sample calculation
inline void invertedSawWave::calculate()
{
    sample = phase *-1 + 1;
}


#endif

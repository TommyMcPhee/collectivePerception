#include "sinOsc.hpp"

sinOsc::sinOsc() {
    
}
sinOsc::sinOsc(float _freq, float _phase, float _amp, int _sampleRate) {
    frequency = _freq;
    phase = _phase;
    amplitude = _amp;
    sampleRate = _sampleRate;
    calculatePhaseInc();
}
float sinOsc::getSample()
{
    float sample;
    checkPhase();
    sample = sin(phase) * amplitude;
    incrementPhase();
    return sample;
}

void sinOsc::setFreq(float _freq){
        frequency = _freq;
        calculatePhaseInc();
    }

void sinOsc::setAmp(float _amp){
    amplitude = _amp;
}

void sinOsc::setPhase(float _phase) {
    phase = _phase;
}

void sinOsc::setSampleRate(int _sampleRate) {
    sampleRate = _sampleRate;
    calculatePhaseInc();
}

float sinOsc::getPhaseInc() {
    return phaseInc;
}

void sinOsc::calculatePhaseInc() {
    phaseInc = TWO_PI * frequency / (float)sampleRate;
};

void sinOsc::checkPhase() {
    if(phase > TWO_PI) { phase -= TWO_PI; }
}

void sinOsc::incrementPhase(){
    phase += phaseInc;
}

float sinOsc::getFrequency(){
    return frequency;
}

float sinOsc::getAmplitude() {
    return amplitude;
}

float sinOsc::getPhase() {
    return phase;
}

int sinOsc::getSampleRate() {
    return sampleRate;
}

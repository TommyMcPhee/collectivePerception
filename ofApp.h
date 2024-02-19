#pragma once

#include "ofMain.h"
#include "sinOsc.hpp"
#include <fstream>

class ofApp : public ofBaseApp{

	public:
		void setup();
		void audioSetup();
		float randomPhase();
		void videoSetup();
		void ofSoundStreamSetup(ofSoundStreamSettings& settings);
		void audioOut(ofSoundBuffer &audioBuffer);
		void renderSample();
		inline float unipolar(float input);
		inline float left(float input, float pan);
		inline float right(float input, float pan);
		inline float modulateTwo(float inputA, float inputB, float ring, float amplitude);
		inline float modulateThree(float inputA, float inputB, float inputC, float ring);
		inline float averageTwo(float inputA, float inputB);
		inline float averageThree(float inputA, float inputB, float inputC);
		void draw();
		void refresh();
		void setUniforms();
		float scaleFrequency(float input);
		void setupWav();
		void writeToFile(ofstream& file, int value, int size);
		void recordSample(int channel);
		void exit();

		sinOsc redLFO;
		sinOsc greenLFO;
		sinOsc blueLFO;
		sinOsc modulatorA;
		sinOsc modulatorB;
		sinOsc modulatorC;
		array<sinOsc, 2> carrierA;
		array<sinOsc, 2> carrierB;
		array<sinOsc, 2> carrierC;
		const int sampleRate = 48000;
		const int bufferSize = 256;
		const int channels = 2;
		const float bankSize = 800.0;
		ofSoundStreamSettings settings;
		ofSoundStream stream;
		float nyquist;
		float fundamentalFrequency;
		float ratio;
		float frequencyIncrement;
		float lowFrequencyIncrement;
		float frequencyA;
		float frequencyB;
		float frequencyC;
		float lowFrequencyA;
		float lowFrequencyB;
		float lowFrequencyC;
		float redSample;
		float greenSample;
		float blueSample;
		float modulatorASample;
		float modulatorBSample;
		float modulatorCSample;
		float modulatorAPan = 0.5;
		float modulatorBPan = 0.5;
		float modulatorCPan = 0.5;
		float modulatorAAmplitude = 0.5;
		float modulatorBAmplitude = 0.5;
		float modulatorCAmplitude = 0.5;
		float carrierAAmplitude = 0.5;
		float carrierBAmplitude = 0.5;
		float carrierCAmplitude = 0.5;
		float carrierAPan = 0.5;
		float carrierBPan = 0.5;
		float carrierCPan = 0.5;
		float abRingPan = 0.5;
		float acRingPan = 0.5;
		float bcRingPan = 0.5;
		float abRing = 0.5;
		float acRing = 0.5;
		float bcRing = 0.5;
		float abPan = 0.5;
		float acPan = 0.5;
		float bcPan = 0.5;
		float abAmplitude = 0.5;
		float acAmplitude = 0.5;
		float bcAmplitude = 0.5;
		float ringPan = 0.5;
		float ring = 0.5;
		array<float, 2> indexA;
		array<float, 2> indexB;
		array<float, 2> indexC;
		array<float, 2> carrierASample;
		array<float, 2> carrierBSample;
		array<float, 2> carrierCSample;
		array<float, 2> sampleA;
		array<float, 2> sampleB;
		array<float, 2> sampleC;
		array<float, 2> sampleAB;
		array<float, 2> sampleAC;
		array<float, 2> sampleBC;
		array<float, 2> sample;
		array<float, 2> lastSample = { 0.0, 0.0 };

		float lowPass;

		float filterRingPan = 0.5;
		float filterRing = 0.7;
		float filterPan = 0.5;
		float filterAmplitude = 1.0;

		ofFbo videoBuffer;
		ofShader shader;
		const int frameRate = 60;
		float width;
		float height;
		ofVec2f window;
		ofVec3f frequency;
		ofVec3f index;
		ofVec3f amplitude;
		ofVec3f x;
		ofVec3f y;
		ofVec4f ab;
		ofVec4f ac;
		ofVec4f bc;
		ofVec2f abc;

		ofstream wavFile;
		const int byteDepth = 2;
		int preAudioP;
		int postAudioP;
		int sampleInt;
		int maxSampleInt;
};

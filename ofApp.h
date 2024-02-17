#pragma once

#include "ofMain.h"
#include "sinOsc.hpp"
#include <fstream>

class ofApp : public ofBaseApp{

	public:
		void setup();
		void audioSetup();
		void videoSetup();
		void ofSoundStreamSetup(ofSoundStreamSettings& settings);
		void audioOut(ofSoundBuffer &audioBuffer);
		void renderSample();
		inline float unipolar(float input);
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
		sinOsc carrierA;
		sinOsc carrierB;
		sinOsc carrierC;
		const int sampleRate = 48000;
		const int bufferSize = 256;
		const int channels = 2;
		ofSoundStreamSettings settings;
		ofSoundStream stream;
		float nyquist;
		float frameRate;
		float fundamentalFrequency;
		float ratio;
		float frequencyIncrement;
		float frequencyA;
		float frequencyB;
		float frequencyC;
		float indexA;
		float indexB;
		float indexC;
		float redSample;
		float greenSample;
		float blueSample;
		array<float, 2> modulatorASample;
		array<float, 2> modulatorBSample;
		array<float, 2> modulatorCSample;
		array<float, 2> carrierASample;
		array<float, 2> carrierBSample;
		array<float, 2> carrierCSample;
		const float bankSize = 800.0;
		array<array<float, 2>, 800> sampleBank;
		array<float, 2> sample;

		ofFbo videoBuffer;
		ofShader shader;
		const int frameRate = 60;
		float width;
		float height;
		ofVec2f window;
		ofVec3f frequency;
		ofVec3f index;
		ofVec3f ring;
		ofVec3f color;
		ofVec4f z0;

		ofstream wavFile;
		const int byteDepth = 2;
		int preAudioP;
		int postAudioP;
		int sampleInt;
		int maxSampleInt;
};

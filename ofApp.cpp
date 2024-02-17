#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	setupWav();
	audioSetup();
	videoSetup();
}

void ofApp::audioSetup() {
	settings.setOutListener(this);
	settings.sampleRate = sampleRate;
	settings.bufferSize = bufferSize;
	settings.numOutputChannels = channels;
	settings.setApi(ofSoundDevice::Api::MS_DS);
	stream.setup(settings);
	nyquist = (float)sampleRate * 0.5;
	frequencyIncrement = 10.0 / nyquist;
	fundamentalFrequency = (float)frameRate / 9.0;
	ratio = 9.0;
	frequencyA = fundamentalFrequency;
	frequencyB = fundamentalFrequency * 4.0;
	frequencyC = fundamentalFrequency * 7.0;
	redLFO = sinOsc(fundamentalFrequency / 40.0, 0.0, 1.0, sampleRate);
	greenLFO = sinOsc(fundamentalFrequency / 70.0, 0.0, 1.0, sampleRate);
	blueLFO = sinOsc(fundamentalFrequency / 100.0, 0.0, 1.0, sampleRate);
	modulatorA = sinOsc(frequencyA * ratio, 0.0, 1.0, sampleRate);
	modulatorB = sinOsc(frequencyB * ratio * 4.0, 0.0, 1.0, sampleRate);
	modulatorC = sinOsc(frequencyC * ratio, 0.0, 1.0, sampleRate);
	carrierA = sinOsc(frequencyA, 0.0, 1.0, sampleRate);
	carrierB = sinOsc(frequencyB, 0.0, 1.0, sampleRate);
	carrierC = sinOsc(frequencyC, 0.0, 1.0, sampleRate);
	indexA = 40.0;
	indexB = 40.0;
	indexC = 40.0;
}

void ofApp::ofSoundStreamSetup(ofSoundStreamSettings &settings) {

}

void ofApp::videoSetup() {
	ofSetFrameRate(60);
	shader.load("collectivePerceptionShader");
	videoBuffer.allocate(ofGetScreenWidth(), ofGetScreenHeight());
	videoBuffer.clear();
	videoBuffer.begin();
	ofClear(0, 0, 0, 255);
	videoBuffer.end();
}

void ofApp::audioOut(ofSoundBuffer& audioBuffer) {
	for (int a = 0; a < audioBuffer.getNumFrames(); a++) {
		renderSample();
		for (int b = 0; b < 2; b++) {
			audioBuffer[a * 2 + b] = sample[b];
		}
	}
}

void ofApp::renderSample() {
	redSample = unipolar(redLFO.getSample());
	greenSample = unipolar(greenLFO.getSample());
	blueSample = unipolar(blueLFO.getSample());
	indexA = pow(nyquist - frequencyA, redSample * frequencyA / nyquist);
	indexB = pow(nyquist - frequencyB, greenSample * frequencyB / nyquist);
	indexC = pow(nyquist - frequencyC, blueSample * frequencyC / nyquist);
	frequencyA += frequencyIncrement;
	frequencyB += frequencyIncrement;
	frequencyC += frequencyIncrement;
	if (frequencyA > nyquist) {
		exit();
	}
	modulatorA.setFreq(frequencyA * ratio);
	modulatorB.setFreq(frequencyB * ratio);
	modulatorC.setFreq(frequencyC * ratio);
	modulatorASample = modulatorA.getSample();
	modulatorBSample = modulatorB.getSample();
	modulatorCSample = modulatorC.getSample();
	carrierA.setFreq(modulatorASample * frequencyA * indexA + frequencyA);
	carrierB.setFreq(modulatorBSample * frequencyB * indexB + frequencyB);
	carrierC.setFreq(modulatorCSample * frequencyC * indexC + frequencyC);
	carrierASample = carrierA.getSample();
	carrierBSample = carrierB.getSample();
	carrierCSample = carrierC.getSample();
	for (int b = 0; b < 2; b++) {
		//sample[b] = (carrierASample + carrierBSample + carrierCSample) / 3.0;
		sample[b] = carrierASample * carrierBSample * carrierCSample;
	}
}

inline float ofApp::unipolar(float input) {
	return input * 0.5 + 0.5;
}

//--------------------------------------------------------------
void ofApp::draw(){
	refresh();
	videoBuffer.begin();
	shader.begin();
	videoBuffer.draw(0, 0);
	setUniforms();
	shader.end();
	videoBuffer.end();
	videoBuffer.draw(0, 0);
	//cout << frequencyA << endl;
}

void ofApp::refresh() {
	width = (float)ofGetWidth();
	height = (float)ofGetHeight();
	videoBuffer.allocate(width, height);
	window.set(width, height);
	ofClear(0, 0, 0, 255);
}

void ofApp::setUniforms() {
	shader.setUniform2f("window", window);
	index.set(indexA, indexB, indexC);
	shader.setUniform3f("index", index);
	frequency.set(scaleFrequency(frequencyA), scaleFrequency(frequencyB), scaleFrequency(frequencyC));
	shader.setUniform3f("frequency", frequency);
	color.set(redSample, greenSample, blueSample);
	shader.setUniform3f("color", color);
	z0.set(0.0, 0.0, 1.0, 1.0);
	shader.setUniform4f("z0", z0);
}

float ofApp::scaleFrequency(float input) {
	return input / (fundamentalFrequency * 9.0);
}

void ofApp::setupWav() {
	wavFile.open("collectivePerceptionRecording.wav", ios::binary);
	wavFile << "RIFF";
	wavFile << "----";
	wavFile << "WAVE";
	wavFile << "fmt ";
	writeToFile(wavFile, byteDepth * 8, 4);
	writeToFile(wavFile, 1, 2);
	writeToFile(wavFile, channels, 2);
	writeToFile(wavFile, sampleRate, 4);
	writeToFile(wavFile, sampleRate * byteDepth, 4);
	writeToFile(wavFile, byteDepth, 2);
	writeToFile(wavFile, byteDepth * 8, 2);
	wavFile << "data";
	wavFile << "----";
	preAudioP = wavFile.tellp();
	maxSampleInt = pow(2, byteDepth * 8 - 1) - 1;
}

void ofApp::writeToFile(ofstream& file, int value, int size) {
	file.write(reinterpret_cast<const char*> (&value), size);
}

void ofApp::recordSample(int channel) {
	sampleInt = static_cast<int>(sample[channel] * maxSampleInt);
	wavFile.write(reinterpret_cast<char*> (&sampleInt), byteDepth);
}

void ofApp::exit() {

}
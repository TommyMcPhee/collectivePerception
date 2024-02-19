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
	frequencyIncrement = 72.0 / nyquist;
	lowFrequencyIncrement = frequencyIncrement / 1000.0;
	fundamentalFrequency = (float)frameRate / 9.0;
	frequencyA = fundamentalFrequency;
	frequencyB = fundamentalFrequency * 4.0;
	frequencyC = fundamentalFrequency * 7.0;
	lowFrequencyA = frequencyA;
	lowFrequencyB = frequencyB;
	lowFrequencyC = frequencyC;
	ratio = 9.0;
	redLFO = sinOsc(fundamentalFrequency / 100.0, randomPhase(), 1.0, sampleRate);
	greenLFO = sinOsc(fundamentalFrequency * 4.0 / 100.0, randomPhase(), 1.0, sampleRate);
	blueLFO = sinOsc(fundamentalFrequency * 7.0/ 100.0, randomPhase(), 1.0, sampleRate);
	blackLFO = sinOsc(0.0, randomPhase(), 1.0, sampleRate);
	modulatorA = sinOsc(frequencyA * ratio, 0.0, 1.0, sampleRate);
	modulatorB = sinOsc(frequencyB * ratio * 4.0, 0.0, 1.0, sampleRate);
	modulatorC = sinOsc(frequencyC * ratio, 0.0, 1.0, sampleRate);
	for (int a = 0; a < 2; a++) {
		carrierA[a] = sinOsc(frequencyA, 0.0, 1.0, sampleRate);
		carrierB[a] = sinOsc(frequencyB, 0.0, 1.0, sampleRate);
		carrierC[a] = sinOsc(frequencyC, 0.0, 1.0, sampleRate);
		indexA[a] = 40.0;
		indexB[a] = 40.0;
		indexC[a] = 40.0;
	}
}

float ofApp::randomPhase() {
	return ofRandomf() * TWO_PI;
}

void ofApp::ofSoundStreamSetup(ofSoundStreamSettings &settings) {

}

void ofApp::videoSetup() {
	ofSetFrameRate(frameRate);
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
			lastSampleA[b] = sampleA[b];
			lastSampleB[b] = sampleB[b];
			lastSampleC[b] = sampleC[b];
			lastSampleAB[b] = sampleAB[b];
			lastSampleAC[b] = sampleAC[b];
			lastSampleBC[b] = sampleBC[b];
			lastSample[b] = sample[b];
		}
	}
}

void ofApp::renderSample() {
	lowFrequencyA -= lowFrequencyIncrement;
	lowFrequencyB -= lowFrequencyIncrement;
	lowFrequencyC -= lowFrequencyIncrement;
	frequencyA += frequencyIncrement;
	frequencyB += frequencyIncrement;
	frequencyC += frequencyIncrement;
	progress = pow((frequencyA - fundamentalFrequency) / (nyquist - fundamentalFrequency), 0.5);
	if (frequencyA > nyquist) {
		exit();
	}
	redLFO.setFreq(lowFrequencyA);
	greenLFO.setFreq(lowFrequencyB);
	blueLFO.setFreq(lowFrequencyC);
	redSample = unipolar(redLFO.getSample());
	greenSample = unipolar(greenLFO.getSample());
	blueSample = unipolar(blueLFO.getSample());
	modulatorAAmplitude = pow(nyquist - frequencyA, averageTwo(greenSample, blueSample) * frequencyA / nyquist);
	modulatorBAmplitude = pow(nyquist - frequencyB, averageTwo(redSample, blueSample) * frequencyB / nyquist);
	modulatorCAmplitude = pow(nyquist - frequencyC, averageTwo(redSample, greenSample) * frequencyC / nyquist);
	modulatorAPan = redSample;
	modulatorBPan = greenSample;
	modulatorCPan = blueSample;	
	carrierAPan = averageTwo(greenSample, blueSample);
	carrierBPan = averageTwo(redSample, blueSample);
	carrierCPan = averageTwo(redSample, greenSample);
	carrierAAmplitude = abs(greenSample - blueSample);
	carrierBAmplitude = abs(redSample - blueSample);
	carrierCAmplitude = abs(redSample - greenSample);
	abRing = averageTwo(1.0 - (redSample * greenSample), blueSample);
	acRing = averageTwo(1.0 - (redSample * blueSample), greenSample);
	bcRing = averageTwo(1.0 - (greenSample * blueSample), redSample);
	abRingPan = abs(averageTwo(redSample, greenSample) - blueSample);
	acRingPan = abs(averageTwo(redSample, blueSample) - greenSample);
	bcRingPan = abs(averageTwo(greenSample, blueSample) - redSample);
	abAmplitude = abs(redSample * greenSample - blueSample);
	acAmplitude = abs(redSample * blueSample - greenSample);
	bcAmplitude = abs(greenSample * blueSample - redSample);
	abPan = averageTwo(abs(redSample - greenSample), blueSample);
	acPan = averageTwo(abs(redSample - blueSample), greenSample);
	bcPan = averageTwo(abs(greenSample - blueSample), redSample);
	ring = 1.0 - (redSample * greenSample * blueSample);
	ringPan = averageThree(redSample, greenSample, blueSample);
	blackLFO.setFreq(abs(1.0 - ring) * fundamentalFrequency);
	blackSample = unipolar(blackLFO.getSample());
	aLowRingPan = averageTwo(carrierAPan, blackSample);
	bLowRingPan = averageTwo(carrierBPan, blackSample);
	cLowRingPan = averageTwo(carrierCPan, blackSample);
	aLowRing = averageTwo(carrierAAmplitude, blackSample);
	bLowRing = averageTwo(carrierBAmplitude, blackSample);
	cLowRing = averageTwo(carrierCAmplitude, blackSample);
	aLowPassPan = averageTwo(modulatorAPan, blackSample);
	bLowPassPan = averageTwo(modulatorBPan, blackSample);
	cLowPassPan = averageTwo(modulatorCPan, blackSample);
	aLowPass = averageTwo(modulatorAAmplitude, blackSample);
	bLowPass = averageTwo(modulatorBAmplitude, blackSample);
	cLowPass = averageTwo(modulatorCAmplitude, blackSample);
	aHighRingPan = abs(carrierAPan - blackSample);
	bHighRingPan = abs(carrierBPan - blackSample);
	cHighRingPan = abs(carrierCPan - blackSample);
	aHighRing = abs(carrierAAmplitude - blackSample);
	bHighRing = abs(carrierBAmplitude - blackSample);
	cHighRing = abs(carrierCAmplitude - blackSample);
	aHighPassPan = abs(modulatorAPan - blackSample);
	bHighPassPan = abs(modulatorBPan - blackSample);
	cHighPassPan = abs(modulatorCPan - blackSample);
	aHighPass = abs(modulatorAAmplitude - blackSample);
	bHighPass = abs(modulatorBAmplitude - blackSample);
	cHighPass = abs(modulatorCAmplitude - blackSample);
	abLowRingPan = averageTwo(abPan, blackSample);
	acLowRingPan = averageTwo(acPan, blackSample);
	bcLowRingPan = averageTwo(bcPan, blackSample);
	abLowRing = averageTwo(abAmplitude, blackSample);
	acLowRing = averageTwo(acAmplitude, blackSample);
	bcLowRing = averageTwo(bcAmplitude, blackSample);
	abLowPassPan = averageTwo(abRingPan, blackSample);
	acLowPassPan = averageTwo(acRingPan, blackSample);
	bcLowPassPan = averageTwo(bcRingPan, blackSample);
	abLowPass = averageTwo(abRing, blackSample);
	acLowPass = averageTwo(acRing, blackSample);
	bcLowPass = averageTwo(bcRing, blackSample);
	abHighRingPan = abs(abPan - blackSample);
	acHighRingPan = abs(acPan - blackSample);
	bcHighRingPan = abs(bcPan - blackSample);
	abHighRing = abs(abAmplitude - blackSample);
	acHighRing = abs(acAmplitude - blackSample);
	bcHighRing = abs(bcAmplitude - blackSample);
	abHighPassPan = abs(abRingPan - blackSample);
	acHighPassPan = abs(acRingPan - blackSample);
	bcHighPassPan = abs(bcRingPan - blackSample);
	abHighPass = abs(abRing - blackSample);
	acHighPass = abs(acRing - blackSample);
	bcHighPass = abs(bcRing - blackSample);
	lowRingPan = averageTwo(ring, blackSample);
	lowRing = 1.0 - (ringPan * blackSample);
	lowPassPan = averageTwo(ringPan, blackSample);
	lowPass = 1.0 - (ring * blackSample);
	highRingPan = abs(ring - blackSample);
	highRing = ringPan * blackSample;
	highPassPan = abs(ringPan - blackSample);
	highPass = ring * blackSample;
	modulatorA.setFreq(frequencyA * ratio);
	modulatorB.setFreq(frequencyB * ratio);
	modulatorC.setFreq(frequencyC * ratio);
	modulatorASample = modulatorA.getSample();
	modulatorBSample = modulatorB.getSample();
	modulatorCSample = modulatorC.getSample();
	indexA[0] = left(modulatorAAmplitude, modulatorAPan);
	indexA[1] = right(modulatorAAmplitude, modulatorAPan);
	indexB[0] = left(modulatorBAmplitude, modulatorBPan);
	indexB[1] = right(modulatorBAmplitude, modulatorBPan);
	indexC[0] = left(modulatorCAmplitude, modulatorCPan);
	indexC[1] = right(modulatorCAmplitude, modulatorCPan);
	for (int b = 0; b < 2; b++) {
		carrierA[b].setAmp(carrierAAmplitude);
		carrierB[b].setAmp(carrierBAmplitude);
		carrierC[b].setAmp(carrierCAmplitude);
		carrierA[b].setFreq(modulatorASample * frequencyA * indexA[b] + frequencyA);
		carrierB[b].setFreq(modulatorBSample * frequencyB * indexB[b] + frequencyB);
		carrierC[b].setFreq(modulatorCSample * frequencyC * indexC[b] + frequencyC);
		carrierASample[b] = carrierA[b].getSample();
		carrierBSample[b] = carrierB[b].getSample();
		carrierCSample[b] = carrierC[b].getSample();
	}
	sampleA[0] = left(carrierASample[0], carrierAPan);
	sampleA[1] = right(carrierASample[1], carrierAPan);
	sampleB[0] = left(carrierBSample[0], carrierBPan);
	sampleB[1] = right(carrierBSample[0], carrierBPan);
	sampleC[0] = left(carrierCSample[0], carrierCPan);
	sampleABMix[0] = modulateTwo(carrierASample[0], carrierBSample[0], left(abRing, abRingPan), left(abAmplitude, abPan));
	sampleABMix[1] = modulateTwo(carrierASample[1], carrierBSample[1], right(abRing, abRingPan), right(abAmplitude, abPan));
	sampleACMix[0] = modulateTwo(carrierASample[0], carrierCSample[0], left(acRing, acRingPan), left(acAmplitude, acPan));
	sampleACMix[1] = modulateTwo(carrierASample[1], carrierCSample[1], right(acRing, acRingPan), right(acAmplitude, acPan));
	sampleBCMix[0] = modulateTwo(carrierBSample[0], carrierCSample[0], left(bcRing, bcRingPan), left(bcAmplitude, bcPan));
	sampleBCMix[1] = modulateTwo(carrierBSample[1], carrierCSample[1], right(bcRing, bcRingPan), right(bcAmplitude, bcPan));
	sampleAB[0] = filter(sampleABMix[0], lastSampleAB[0], left(abLowRing, abLowRingPan), left(abLowPass, abLowPassPan), left(abHighRing, abHighRingPan), left(abHighPass, abHighPassPan));
	sampleAB[1] = filter(sampleABMix[1], lastSampleAB[1], right(abLowRing, abLowRingPan), right(abLowPass, abLowPassPan), right(abHighRing, abHighRingPan), right(abHighPass, abHighPassPan));
	sampleAC[0] = filter(sampleACMix[0], lastSampleAC[0], left(acLowRing, acLowRingPan), left(acLowPass, acLowPassPan), left(acHighRing, acHighRingPan), left(acHighPass, acHighPassPan));
	sampleAC[1] = filter(sampleACMix[1], lastSampleAC[1], right(acLowRing, acLowRingPan), right(acLowPass, acLowPassPan), right(acHighRing, acHighRingPan), right(acHighPass, acHighPassPan));
	sampleBC[0] = filter(sampleBCMix[0], lastSampleBC[0], left(bcLowRing, bcLowRingPan), left(bcLowPass, bcLowPassPan), left(bcHighRing, bcHighRingPan), left(bcHighPass, bcHighPassPan));
	sampleBC[1] = filter(sampleBCMix[1], lastSampleBC[1], right(bcLowRing, bcLowRingPan), right(bcLowPass, bcLowPassPan), right(bcHighRing, bcHighRingPan), right(bcHighPass, bcHighPassPan));
	sampleMix[0] = modulateThree(sampleAB[0], sampleAC[0], sampleBC[0], left(ring, ringPan));
	sampleMix[1] = modulateThree(sampleAB[1], sampleAC[1], sampleBC[1], right(ring, ringPan));
	sample[0] = filter(sampleMix[0], lastSample[0], left(lowRing, lowRingPan), left(lowPass, lowPassPan), left(highRing, highRingPan), left(highPass, highPassPan));
	sample[1] = filter(sampleMix[1], lastSample[1], right(lowRing, lowRingPan), right(lowPass, lowPassPan), right(highRing, highRingPan), right(highPass, highPassPan));
}

inline float ofApp::unipolar(float input) {
	return input * 0.5 + 0.5;
}

inline float ofApp::left(float input, float pan) {
	return sqrt(1.0 - pan) * input;
}

inline float ofApp::right(float input, float pan) {
	return sqrt(pan) * input;
}

inline float ofApp::modulateTwo(float inputA, float inputB, float ring, float amplitude) {
	return amplitude * (inputA * inputB * ring + averageTwo(inputA, inputB)) / (1.0 + (1.0 - ring));
}

inline float ofApp::modulateThree(float inputA, float inputB, float inputC, float ring) {
	return (inputA * inputB * inputC * ring + averageThree(inputA, inputB, inputC)) / (1.0 + (1.0 - ring));
}

inline float ofApp::averageTwo(float inputA, float inputB) {
	return (inputA + inputB) / 2.0;
}

inline float ofApp::averageThree(float inputA, float inputB, float inputC) {
	return (inputA + inputB + inputC) / 3.0;
}

inline float ofApp::filter(float z1, float z0, float lowRing, float lowPass, float highRing, float highPass) {
	lowPass = pow(lowPass, progress * progress);
	highPass = pow(highPass, progress);
	float unamplified = averageTwo(modulateTwo(z1, z0, lowRing, lowPass), modulateTwo(z1, -1.0 * z0, highRing, highPass));
	return ofSign(unamplified) * pow(abs(unamplified), 0.75);
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
}

void ofApp::refresh() {
	width = (float)ofGetWidth();
	height = (float)ofGetHeight();
	videoBuffer.allocate(width, height);
	window.set(width, height);
	ofClear(0, 0, 0, 255);
}

void ofApp::setUniforms() {
	shader.setUniform1f("progress", progress);
	shader.setUniform2f("window", window);
	index.set(modulatorAAmplitude, modulatorBAmplitude, modulatorCAmplitude);
	shader.setUniform3f("index", index);
	frequency.set(scaleFrequency(frequencyA), scaleFrequency(frequencyB), scaleFrequency(frequencyC));
	shader.setUniform3f("frequency", frequency);
	carrier.set(redSample, greenSample, blueSample);
	shader.setUniform3f("carrier", carrier);
	x.set(carrierAPan, carrierBPan, carrierCPan);
	shader.setUniform3f("x", x);
	y.set(abs(modulatorAPan - carrierAPan), (modulatorBPan - carrierBPan), (modulatorCPan - carrierCPan));
	shader.setUniform3f("y", y);
	ab.set(abPan, abs(abPan - abRingPan), abRing, abAmplitude);
	shader.setUniform4f("ab", ab);
	ac.set(acPan, abs(acPan - acRingPan), acRing, acAmplitude);
	shader.setUniform4f("ac", ac);
	bc.set(bcPan, abs(bcPan - bcRingPan), bcRing, bcAmplitude);
	shader.setUniform4f("bc", bc);
	abc.set(ring, ringPan);
	shader.setUniform2f("abc", abc);
	redLow.set(aLowRingPan, aLowRing, aLowPassPan, aLowPass);
	shader.setUniform4f("redLow", redLow);
	redHigh.set(aHighRingPan, aHighRing, aHighPassPan, aHighPass);
	shader.setUniform4f("redHigh", redHigh);
	greenLow.set(bLowRingPan, bLowRing, bLowPassPan, bLowPass);
	shader.setUniform4f("greenLow", greenLow);
	greenHigh.set(bHighRingPan, bHighRing, bHighPassPan, bHighPass);
	shader.setUniform4f("greenHigh", greenHigh);
	blueLow.set(cLowRingPan, cLowRing, cLowPassPan, cLowPass);
	shader.setUniform4f("blueLow", blueLow);
	blueHigh.set(cHighRingPan, cHighRing, cHighPassPan, cHighPass);
	shader.setUniform4f("blueHigh", blueHigh);
	redGreenLow.set(abLowRingPan, abLowRing, abLowPassPan, abLowPass);
	shader.setUniform4f("redGreenLow", redGreenLow);
	redGreenHigh.set(abHighRingPan, abHighRing, abHighPassPan, abHighPass);
	shader.setUniform4f("redGreenHigh", redGreenHigh);
	redBlueLow.set(acLowRingPan, acLowRing, acLowPassPan, acLowPass);
	shader.setUniform4f("redBlueLow", redBlueLow);
	redBlueHigh.set(acHighRingPan, acHighRing, acHighPassPan, acHighPass);
	shader.setUniform4f("redBlueHigh", redBlueHigh);
	greenBlueLow.set(bcLowRingPan, bcLowRing, bcLowPassPan, bcLowPass);
	shader.setUniform4f("greenBlueLow", greenBlueLow);
	greenBlueHigh.set(bcHighRingPan, bcHighRing, bcHighPassPan, bcHighPass);
	shader.setUniform4f("greenBlueHigh", greenBlueHigh);
	low.set(lowRingPan, lowRing, lowPassPan, lowPass);
	shader.setUniform4f("low", low);
	high.set(highRingPan, highRing, highPassPan, highPass);
	shader.setUniform4f("high", high);
}

float ofApp::scaleFrequency(float input) {
	return pow(input * width * height / nyquist, 0.1);
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
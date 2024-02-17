#version 150

uniform sampler2DRect tex0;
//uniform sampler2DRect tex1;
//uniform sampler2DRect tex2;
//uniform sampler2DRect tex3;
in vec2 texCoordVarying;
out vec4 outputColor;

uniform vec2 window;
uniform vec3 frequency;
uniform vec3 index;
uniform vec3 ring;
uniform vec3 color;

uniform vec4 z0;
//uniform vec4 z1;
//uniform vec4 z2;
//uniform vec4 z3;

vec2 normalized;

float pixels;

float unipolar(float input){
    return input * 0.5 + 0.5;
}

float bipolar(float input){
    return (input - 0.5) * 2.0;
}

float oscillate(vec2 twoPiNormalized, float _frequency, float _index, float _ring, float _amplitude){
    float phaseX = _frequency * twoPiNormalized.x;
    float phaseY = _frequency * twoPiNormalized.y;
    float modulatorX = cos(phaseX) * _index;
    float modulatorY = cos(phaseY) * _index;
    float carrierFrequencyX = (modulatorX + _frequency) / 9.0;
    float carrierFrequencyY = (modulatorY + _frequency) / 9.0;
    float carrierX = unipolar(cos(phaseX * carrierFrequencyX));
    float carrierY = unipolar(cos(phaseY * carrierFrequencyY));
    float mixedOutput = mix(carrierX, carrierY, 0.5);
    float ringOutput = carrierX * carrierY;
    return mix(mixedOutput, ringOutput, _ring) * _amplitude;
}

vec2 translate(vec2 texCoord, vec2 window, vec4 translation){
    return vec2((translation.x * window.x + texCoord.x) * translation.z, (translation.y * window.y + texCoord.y) * translation.w);
}

/*
uniform vec2 oscillators;
uniform vec4 color;
uniform vec4 translate;

float powerScale(float input){
    float scale = pow(pixels, 0.25);
    return pow(input, 1.0 + pow(scale, 1.0 / pow(scale, 0.5))) * scale;
}

float scalePhase(float phase, float skew){
    float scaledSkew = powerScale(skew);
    return pow(phase, scaledSkew) * TWO_PI;
}

float scaleTension(float tension){
    return pow(tension, 2.0) * 2.0;
}

float lfo(float phase, float skew, float depth, float offset, float tension){
    float scaledOffset = offset * TWO_PI;
    float scaledPhase = scalePhase(phase, skew);
    float sine = sin(scaledPhase + scaledOffset);
    float scaledTension = scaleTension(tension);
    float scaledDepth = powerScale(depth);
    return pow(unipolar(abs(sine) * sign(sine)), scaledDepth);
}

float oscillate(float phase, float skew, float depth, float frequency, float tension){
    float scaledPhase = scalePhase(phase, skew);
    float scaledFrequency = pow(frequency, 0.5) * pixels;
    float sine = sin(scaledPhase * TWO_PI * frequency);
    float scaledTension = scaleTension(tension);
    float scaledDepth = powerScale(depth);
    return pow(unipolar(abs(sine) * sign(sine)), scaledDepth);
}

float averageTwo(float a, float b){
    return (a + b) / 2.0;
}

float normalizedAdd(float a, float b){
    return (a + b) / (1.0 + b);
}
*/
void main()
{
    vec4 buffer0 = texture2DRect(tex0, translate(texCoordVarying, window, z0));   
    vec2 normalized = gl_FragCoord.xy / window;
    //pixels = window.x * window.y;
    float pi = radians(180.0);
    vec2 twoPiNormalized = normalized * 2.0 * pi - vec2(pi, pi);
    float red = oscillate(twoPiNormalized, frequency.r, index.r, ring.r, color.r);
    float green = oscillate(twoPiNormalized, frequency.g, index.g, ring.g, color.g);
    float blue = oscillate(twoPiNormalized, frequency.b, index.b, ring.b, color.b);
    outputColor = mix(vec4(red, green, blue, 1.0), buffer0, 0.33);
}
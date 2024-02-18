#version 150

out vec4 outputColor;

uniform vec2 window;
uniform vec3 index;
uniform vec3 frequency;
uniform vec3 amplitude;
uniform vec3 x;
uniform vec3 y;
uniform vec4 ab;
uniform vec4 ac;
uniform vec4 bc;
uniform vec2 abc;

float unipolar(float input){
    return input * 0.5 + 0.5;
}

float bipolar(float input){
    return (input - 0.5) * 2.0;
}

float oscillate(vec2 twoPiNormalized, float _frequency, float _index, float _amplitude){
    float phaseX = _frequency * twoPiNormalized.x;
    float phaseY = _frequency * twoPiNormalized.y;
    float modulatorX = cos(phaseX) * _index;
    float modulatorY = cos(phaseY) * _index;
    float carrierFrequencyX = (modulatorX + _frequency) / 9.0;
    float carrierFrequencyY = (modulatorY + _frequency) / 9.0;
    float carrierX = unipolar(cos(phaseX * carrierFrequencyX));
    float carrierY = unipolar(cos(phaseY * carrierFrequencyY));
    float mixedOutput = mix(carrierX, carrierY, 0.5);
    return mixedOutput * _amplitude;
}

vec2 modulate(float a, float b, float index){
    return mix(vec2(a, b), vec2(a * b), index);
}

float distribute(vec2 xy, vec2 normalized){
    return mix(mix(1.0 - normalized.x, normalized.x, xy.x), mix(1.0 - normalized.y, normalized.y, xy.y), 0.5);
}

void main()
{ 
    vec2 normalized = gl_FragCoord.xy / window;
    float pi = radians(180.0);
    vec2 twoPiNormalized = normalized * 2.0 * pi - vec2(pi, pi);
    float red = oscillate(twoPiNormalized, frequency.r, index.r, amplitude.r) * distribute(vec2(x.r, y.r), normalized);
    float green = oscillate(twoPiNormalized, frequency.g, index.g, amplitude.g) * distribute(vec2(x.g, y.g), normalized);
    float blue = oscillate(twoPiNormalized, frequency.b, index.b, amplitude.b) * distribute(vec2(x.g, y.b), normalized);
    vec2 redGreen = modulate(red, green, ab.z) * distribute(ab.xy, normalized) * ab.w;
    vec2 redBlue = modulate(red, blue, ac.z) * distribute(ac.xy, normalized) * ac.w;
    vec2 greenBlue = modulate(green, blue, bc.z) * distribute(bc.xy, normalized) * bc.w;
    vec3 color = vec3(mix(redGreen.x, redBlue.x, 0.5), mix(redGreen.y, greenBlue.x, 0.5), mix(redBlue.y, greenBlue.y, 0.5)) * distribute(abc, normalized);
    outputColor = vec4(newColor, 1.0);
    
    //outputColor = vec4(1.0, 0.0, 0.0, 1.0);
}
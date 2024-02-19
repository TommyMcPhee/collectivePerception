#version 150

uniform sampler2DRect tex0;
in vec2 texCoordVarying;
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
    return mix(vec2(a, b), vec2(pow(a * b, 0.25)), index);
}

float distribute(vec2 xy, vec2 normalized, float decenterFloat){
    return mix(pow((1.0 - abs(xy.x - normalized.x)) * (1.0 - abs(xy.y - normalized.y)), 0.25), decenterFloat * pow(normalized.y, 0.25), 0.5);
}

void main()
{ 
    vec4 old = texture2DRect(tex0, texCoordVarying);
    //vec3 filter = vec3(mix(old.rgb, 1.0 - old.rgb, 0.7));
    vec2 normalized = gl_FragCoord.xy / window;
    vec2 decenter = vec2(abs(normalized.x - 0.5) * 2.0, abs(normalized.y - 0.5) * 2.0);
    float decenterFloat = mix(decenter.x, decenter.y, 0.5);
    float pi = radians(180.0);
    vec2 twoPiNormalized = normalized * 2.0 * pi - vec2(pi, pi);
    float red = oscillate(twoPiNormalized, frequency.r, index.r, amplitude.r) * distribute(vec2(x.r, y.r), normalized, decenterFloat);
    float green = oscillate(twoPiNormalized, frequency.g, index.g, amplitude.g) * distribute(vec2(x.g, y.g), normalized, decenterFloat);
    float blue = oscillate(twoPiNormalized, frequency.b, index.b, amplitude.b) * distribute(vec2(x.g, y.b), normalized, decenterFloat);
    //float filteredRed = mix(red * old.r, red + old.r, 0.25);
    //float filteredGreen = mix(green * old.g, green + old.g, 0.25);
    //float filteredBlue = mix(blue * old.b, blue + old.b, 0.25);
    vec2 redGreen = abs(vec2(blue) - modulate(red, green, ab.z) * distribute(ab.xy, normalized, decenterFloat) * ab.w);
    vec2 redBlue = abs(vec2(green) - modulate(red, blue, ac.z) * distribute(ac.xy, normalized, decenterFloat) * ac.w);
    vec2 greenBlue = abs(vec2(red) - modulate(green, blue, bc.z) * distribute(bc.xy, normalized, decenterFloat) * bc.w);
    vec3 new = vec3(redGreen.x * redBlue.x, redGreen.y * greenBlue.x, redBlue.y * greenBlue.y) * distribute(abc, normalized, decenterFloat);
    //outputColor = vec4(mix(filter * new, filter + new, 0.25), 1.0);
    outputColor = vec4(new, 1.0);
    
    //outputColor = vec4(1.0, 0.0, 0.0, 1.0);
}
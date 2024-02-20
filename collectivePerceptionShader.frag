#version 150

uniform sampler2DRect tex0;
in vec2 texCoordVarying;
out vec4 outputColor;
uniform float progress;
uniform vec2 window;
uniform vec3 index;
uniform vec3 frequency;
uniform vec3 carrier;
uniform vec3 x;
uniform vec3 y;
uniform vec4 ab;
uniform vec4 ac;
uniform vec4 bc;
uniform vec2 abc;
uniform vec4 redLow;
uniform vec4 redHigh;
uniform vec4 greenLow;
uniform vec4 greenHigh;
uniform vec4 blueLow;
uniform vec4 blueHigh;
uniform vec4 redGreenLow;
uniform vec4 redGreenHigh;
uniform vec4 redBlueLow;
uniform vec4 redBlueHigh;
uniform vec4 greenBlueLow;
uniform vec4 greenBlueHigh;
uniform vec4 low;
uniform vec4 high;

float unipolar(float a){
    return a * 0.5 + 0.5;
}

float bipolar(float a){
    return (a - 0.5) * 2.0;
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
    return mix(pow((1.0 - abs(xy.x - normalized.x)) * (1.0 - abs(xy.y - normalized.y)), 0.25), decenterFloat * normalized.y * normalized.y, 0.25);
}

float pan(float x, float normalizedX){
    return pow(1.0 - abs(x - normalizedX), 0.5);
}

float pass(float z1, float z0, vec4 lowParameters, vec4 highParameters, float normalizedX){
    float inverseZ = 1.0 - z0;
    float lowPass = mix(mix(z1 * z0 * pan(lowParameters.x, normalizedX), mix(z1, z0, 0.5) * pan(lowParameters.z, normalizedX), lowParameters.y), z1, lowParameters.w);
    float highPass = mix(mix(z1 * inverseZ * pan(highParameters.x, normalizedX), mix(z1, inverseZ, 0.5) * pan(highParameters.z, normalizedX), highParameters.y), z1, highParameters.w);
    return mix(mix(lowPass, highPass, 0.5), lowPass * highPass, pow(progress, 0.5));
}

void main()
{ 
    vec4 old = texture2DRect(tex0, texCoordVarying);
    vec3 last = old.rgb;
    vec2 normalized = gl_FragCoord.xy / window;
    vec2 decenter = vec2(abs(normalized.x - 0.5) * 2.0, abs(normalized.y - 0.5) * 2.0);
    float decenterFloat = pow(mix(decenter.x, decenter.y, 0.5), 4.0);
    float pi = radians(180.0);
    vec2 twoPiNormalized = normalized * 2.0 * pi - vec2(pi, pi);
    float redMix = oscillate(twoPiNormalized, frequency.r, index.r, carrier.r) * distribute(vec2(x.r, y.r), normalized, decenterFloat);
    float greenMix = oscillate(twoPiNormalized, frequency.g, index.g, carrier.g) * distribute(vec2(x.g, y.g), normalized, decenterFloat);
    float blueMix = oscillate(twoPiNormalized, frequency.b, index.b, carrier.b) * distribute(vec2(x.g, y.b), normalized, decenterFloat);
    float red = pass(redMix, last.r, redLow, redHigh, normalized.x);
    float green = pass(greenMix, last.g, greenLow, greenHigh, normalized.x);
    float blue = pass(blueMix, last.b, blueLow, blueHigh, normalized.x);
    vec2 redGreenMix = abs(modulate(red, green, ab.z) * distribute(ab.xy, normalized, decenterFloat) * ab.w);
    vec2 redBlueMix = abs(modulate(red, blue, ac.z) * distribute(ac.xy, normalized, decenterFloat) * ac.w);
    vec2 greenBlueMix = abs(modulate(green, blue, bc.z) * distribute(bc.xy, normalized, decenterFloat) * bc.w);  
    vec2 redGreen = vec2(pass(redGreenMix.x, last.r, redGreenLow, redGreenHigh, normalized.x), pass(redGreenMix.x, last.g, redGreenLow, redGreenHigh, normalized.x)) - vec2(blue);
    vec2 redBlue = vec2(pass(redBlueMix.x, last.r, redBlueLow, redBlueHigh, normalized.x), pass(redBlueMix.y, last.b, redBlueLow, redBlueHigh, normalized.x)) - vec2(green);
    vec2 greenBlue = vec2(pass(greenBlueMix.x, last.g, greenBlueLow, greenBlueHigh, normalized.x), pass(greenBlueMix.y, last.b, greenBlueLow, greenBlueHigh, normalized.x)) - vec2(red);
    vec3 new = vec3(redGreen.x * redBlue.x, redGreen.y * greenBlue.x, redBlue.y * greenBlue.y) * distribute(abc, normalized, decenterFloat);
    vec3 filtered = vec3(pass(new.r, last.r, low, high, normalized.x), pass(new.g, last.g, low, high, normalized.x), pass(new.b, last.b, low, high, normalized.x));
    vec3 white = vec3(filtered.r * filtered.g * filtered.b);
    vec3 nonWhite = 1.0 - white;
    vec3 grey = white * nonWhite;
    vec3 nonGrey = 1.0 - grey;
    vec3 secondary = vec3(((filtered.r * filtered.g) + (filtered.r * filtered.b) + (filtered.g * filtered.b)) / 3.0);
    vec3 nonSecondary = 1.0 - secondary;
    vec3 primary = nonWhite * nonSecondary;
    vec3 tertiary = secondary * primary;
    vec3 extraFilter = vec3(pow(tertiary.r * nonGrey.r, progress), pow(tertiary.g * nonGrey.g, progress), pow(tertiary.b * nonGrey.b, progress));
    vec3 colorFilter = extraFilter * (((1.0 - normalized.y) * nonWhite) + (normalized.y * (1.0 - decenterFloat) * (secondary + primary)) / 3.0);
    float filterPower = 0.11;
    vec3 lastFilter = vec3(pow(last.r, filterPower), pow(last.g, filterPower), pow(last.b, filterPower));
    vec3 processed = colorFilter * mix(nonWhite * filtered, lastFilter * extraFilter * pow(decenterFloat, 2.0) * pow(normalized.y, 3.0), pow(1.0 - sin(progress * pi), 0.0625));
    vec3 color = processed;
    float colorPower = 0.33;
    outputColor = vec4(pow(color.r, colorPower), pow(color.g, colorPower), pow(color.b, colorPower), 1.0);
}
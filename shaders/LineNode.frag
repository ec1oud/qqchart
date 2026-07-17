#version 440

layout(location = 0) in float vT;
layout(location = 1) in vec4 vColor;

layout(location = 0) out vec4 fragColor;

layout(std140, binding = 0) uniform buf {
    mat4 qt_Matrix;                     // Qt's usual transform to go from Item to Window coordinates
    vec4 dataScaleOffset;               // xy: scale from real-world units to pixels; zw: offset in pixels
    vec4 normalColor;                   // color to stroke or fill, if data is within range
    vec4 warningMinColor;
    vec4 warningMaxColor;
    float qt_Opacity;
    float height;                       // total height in pixels
    float lineWidth;                    // in pixels
    float warningBelowMinimum;          // change to warningMinColor if datapoint > warningBelowMinimum
    float warningAboveMaximum;          // change to warningMaxColor if datapoint > warningAboveMaximum
    float fillDirection;                // -1 to fill above, +1 to fill below, 0 to stroke
    float aa;
};

#define PI 3.14159265358979323846

void main(void)
{
    float tt = abs(aa - 1.0) + aa * cos(vT * PI);
    fragColor = normalColor * tt;
}

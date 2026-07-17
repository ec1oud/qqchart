#version 440

layout(location = 0) in float vDist;    // signed perpendicular distance from the line centre, in pixels
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

void main(void)
{
    float coverage;
    if (fillDirection != 0.0 || aa < 0.0) {
        // filled area under/over the curve, or wireframe: solid
        coverage = 1.0;
    } else {
        // Analytic (box-filtered) coverage from the signed distance: fully opaque in the
        // interior, ramping to 0 across a band centred on the nominal edge. aa selects the
        // band width: ~0 gives a hard edge, 1 gives a 1px feather. This keeps the stroke a
        // consistent flat-topped band at any width or angle, and thin (sub-pixel) strokes
        // fade uniformly instead of breaking into dashes.
        float halfWidth = lineWidth * 0.5;
        float aaBand = mix(0.02, 1.0, aa);
        coverage = clamp((halfWidth - abs(vDist)) / aaBand + 0.5, 0.0, 1.0);
    }
    fragColor = normalColor * (coverage * qt_Opacity);
}

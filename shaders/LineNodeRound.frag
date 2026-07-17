#version 440

layout(location = 0) in vec4 vSeg;      // this segment in pixels: xy = start, zw = end
layout(location = 1) in vec2 vPos;      // this fragment's position in pixels

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
        // Distance from this fragment to the segment (a capsule): fully covered within
        // halfWidth, with round caps at each end. Adjacent segments' caps overlap at the
        // shared datapoint, giving a round join that can't pinch at sharp angles. aa selects
        // the edge softness (~0 = hard, 1 = 1px feather); thin strokes fade uniformly.
        vec2 a = vSeg.xy;
        vec2 ba = vSeg.zw - a;
        vec2 pa = vPos - a;
        float h = clamp(dot(pa, ba) / max(dot(ba, ba), 1e-6), 0.0, 1.0);
        float d = length(pa - ba * h);
        float halfWidth = lineWidth * 0.5;
        float aaBand = mix(0.02, 1.0, aa);
        coverage = clamp((halfWidth - d) / aaBand + 0.5, 0.0, 1.0);
    }
    fragColor = normalColor * (coverage * qt_Opacity);
}

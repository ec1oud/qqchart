#version 440

// per-vertex givens
layout(location = 0) in vec4 pos;       // x, y, i, t; x and y are time and value, in real-world units
layout(location = 1) in vec4 prevNext;  // time and value of the previous datapoint, and the next one

// variables which will be passed through to the frag shader, and automatically interpolated between vertices
layout(location = 0) out vec4 vSeg;     // this segment in pixels: xy = start (prev), zw = end (cur)
layout(location = 1) out vec2 vPos;     // this fragment's position in pixels

// constants passed in from LineNode
layout(std140, binding = 0) uniform buf {
    mat4 qt_Matrix;                     // Qt's usual transform to go from Item to Window coordinates
    vec4 dataScaleOffset;               // xy: scale from real-world units to pixels; zw: offset in pixels,
                                        // to locate the y axis and to put the latest sample at the right.
                                        // (This used to be a mat2 + vec2, but Qt's OpenGL RHI backend
                                        // mishandles the std140 column stride of a mat2 in a uniform block;
                                        // the transform is always a pure scale, so a vec4 is enough.)
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

out gl_PerVertex { vec4 gl_Position; };

void main(void)
{
    float i = pos.z;                    // vertex index 0..3 (4 vertices per datapoint)
    float t = pos.w;                    // -1 for the lower vertices, +1 for the upper ones

    if (fillDirection != 0.) {
        // Filling the area above/below the curve: keep the original continuous-ribbon
        // behaviour (odd vertices stay on the line, even vertices go to the baseline).
        float oddMult = mod(i, 2.0);
        float evenMult = mod(i + 1.0, 2.0);
        vec2 posPx = dataScaleOffset.xy * pos.xy;
        posPx += dataScaleOffset.zw;
        posPx.y *= oddMult;
        vec2 offset = vec2(0.);
        if (fillDirection > 0.)
            offset.y = evenMult * height * fillDirection;
        gl_Position = qt_Matrix * vec4(posPx + offset, 0, 1.0);
        vSeg = vec4(0.);
        vPos = vec2(0.);
        return;
    }

    // Stroking. Each datapoint's 4 vertices form a standalone quad (drawn as indexed
    // triangles by LineNode) covering the segment from the previous datapoint to this one,
    // extended half a line width + a 1px feather past each end. The frag shader turns the
    // distance from this segment into coverage, so the segment gets round caps; adjacent
    // segments share an endpoint, so their caps overlap into a round join. No miter, so
    // sharp spikes never pinch and there is no miter limit to enforce.
    float halfLineWidth = lineWidth / 2.0;
    float ext = halfLineWidth + 1.0;    // feather of 1px for the AA ramp

    vec2 cur  = dataScaleOffset.xy * pos.xy      + dataScaleOffset.zw;
    vec2 prev = dataScaleOffset.xy * prevNext.xy + dataScaleOffset.zw;
    vec2 next = dataScaleOffset.xy * prevNext.zw + dataScaleOffset.zw;

    vec2 seg = cur - prev;
    float segLen = length(seg);
    // First datapoint has prev == cur (zero-length segment -> a round dot); pick any valid
    // direction so the quad still bounds the cap disc.
    vec2 dir = segLen > 0.001 ? seg / segLen
             : (length(next - cur) > 0.001 ? normalize(next - cur) : vec2(1.0, 0.0));
    vec2 perp = vec2(-dir.y, dir.x);

    float endB = step(1.5, i);          // 0 for i=0,1 (prev end); 1 for i=2,3 (cur end)
    vec2 endPt = mix(prev, cur, endB);
    float along = endB * 2.0 - 1.0;     // extend prev end backwards, cur end forwards
    vec2 cornerPx = endPt + dir * (along * ext) + perp * (t * ext);

    vSeg = vec4(prev, cur);
    vPos = cornerPx;
    gl_Position = qt_Matrix * vec4(cornerPx, 0, 1.0);
}

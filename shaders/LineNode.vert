#version 440

// per-vertex givens
layout(location = 0) in vec4 pos;       // x, y, i, t; x and y are time and value, in real-world units
layout(location = 1) in vec4 prevNext;  // time and value of the previous datapoint, and the next one
//layout(location = 2) in float t;

// variables which will be passed through to the frag shader, and automatically interpolated between vertices
layout(location = 0) out float vDist;   // signed perpendicular distance from the line centre, in pixels
layout(location = 1) out vec4 vColor;   // color of the vertex

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
    float i = pos.z;                    // vertex index from 0 to 3 (there are 4 vertices per datapoint)
    float t = pos.w;                    // -1 for even (lower) vertices, +1 for odd (upper) vertices (redundant, since we could calculate it from i)
    vec2 posPx = dataScaleOffset.xy * pos.xy; // the data point, transformed to pixel units, in 2D (we don't need 3D)
    float oddMult = mod(i, 2.0);        // will be 1 if i is odd, 0 if it's even
    float evenMult = mod(i + 1.0, 2.0); // will be 0 if i is odd, 1 if it's even
    vec2 offset = vec2(0.);             // how much we will shift the vertex from its on-line position

    // Expand the geometry half a line width PLUS a feather, so there is room for the
    // antialiasing ramp just outside the nominal edge. The frag shader turns the
    // interpolated perpendicular distance (vDist) into coverage, so the stroke reads as
    // a flat-topped band with a 1px soft edge regardless of width or angle.
    float halfLineWidth = lineWidth / 2.0;
    float feather = 1.0;                // extra pixels each side for the AA ramp
    float outer = halfLineWidth + feather;
    vDist = 0.0;

    if (fillDirection == 0.) {
        // we are stroking: need to calculate the miter or knee
        vec2 prev = dataScaleOffset.xy * prevNext.xy;
        vec2 next = dataScaleOffset.xy * prevNext.zw;
        vec2 lineToward = normalize(posPx - prev);
        vec2 lineAway = normalize(next - posPx);
        vec2 normal = vec2(lineAway.y, -lineAway.x);
        vec2 averageTangent = (prev == posPx) ? lineAway : normalize(lineToward + lineAway);
        vec2 miter = normalize(vec2(-averageTangent.y, averageTangent.x));
        float miterLength = outer / dot(normal, miter);
        // Offsetting by miterLength along the miter puts the vertex exactly `outer`
        // pixels from the segment centre, so the perpendicular distance is simply t*outer.
        vDist = t * outer;

        if (dot(lineToward, lineAway) >= 0) {
            // angle is right or obtuse: OK to use ordinary miter
            offset = -t * miterLength * miter;
        } else {
            // angle is acute: make a beveled miter to avoid overshooting too far
            vec2 upToCap = miter * outer * t;
            vec2 capDeviation = averageTangent * outer;
            float innerMiterLimit = max(-lineWidth, min(posPx.x - prev.x, lineWidth)) * 8.;
            miterLength = max(-innerMiterLimit, min(miterLength, innerMiterLimit));
            if (lineToward.y > 0) {
                capDeviation *= sign(i - 2.0) * mod(i - 2.0, 2.0); // lower knee
                offset = oddMult * (upToCap + capDeviation) + evenMult * (-t * miterLength * miter);
            } else {
                capDeviation *= sign(i - 1.0) * mod(i - 1.0, 2.0); // upper knee
                offset = evenMult * (upToCap + capDeviation) + oddMult * (-t * miterLength * miter);
            }
        }
    }

    posPx += dataScaleOffset.zw;

    if (fillDirection != 0.) {
        // we are filling space above or below the line
        // odd vertices stay in place, while even vertices are moved to the top or bottom
        posPx.y *= oddMult;
        if (fillDirection > 0)
            offset.y = evenMult * height * fillDirection;
    }

    // apply the calculated offset to the vertex, and then apply Qt's usual
    // transform to go from Item coordinates to Window coordinates (and in 3D)
    gl_Position = qt_Matrix * vec4(posPx + offset, 0, 1.0);

    vColor = pos.y > warningAboveMaximum ? warningMaxColor :
             pos.y < warningBelowMinimum ? warningMinColor : normalColor;
}

#version 120

// per-vertex givens
attribute highp vec4 pos;               // x, y, i, t; x and y are time and value, in real-world units
attribute highp vec4 prevNext;          // time and value of the previous datapoint, and the next one

// constants passed in from LineNode
uniform lowp float lineWidth;           // in pixels
uniform lowp float warningBelowMinimum; // change to warningMinColor if datapoint > warningBelowMinimum
uniform lowp float warningAboveMaximum; // change to warningMaxColor if datapoint > warningAboveMaximum
uniform lowp float fillDirection;       // -1 to fill above, +1 to fill below, 0 to stroke
uniform lowp vec4 normalColor;          // color to stroke or fill, if data is within range
uniform lowp vec4 warningMinColor;
uniform lowp vec4 warningMaxColor;
uniform highp mat4 dataTransform;       // transform to go from real-world units to pixels
uniform highp mat4 qt_Matrix;           // Qt's usual transform to go from Item to Window coordinates

// variables which will be passed through to the frag shader, and automatically interpolated between vertices
varying lowp float vT;                  // distance: 0 is on the line; goes to +/-0.5 outwards across the stroke
varying lowp vec4 color;                // color of the vertex

void main(void)
{
    float i = pos.z;                    // vertex index from 0 to 3 (there are 4 vertices per datapoint)
    float t = pos.w;                    // -1 for even (lower) vertices, +1 for odd (upper) vertices (redundant, since we could calculate it from i)
    mat2 dataScalingTransform = mat2(dataTransform);
    vec2 posPx = dataScalingTransform * pos.xy; // the data point, transformed to pixel units, in 2D (we don't need 3D)
    float oddMult = mod(i, 2.0);        // will be 1 if i is odd, 0 if it's even
    float evenMult = mod(i + 1.0, 2.0); // will be 0 if i is odd, 1 if it's even
    vec2 offset = vec2(0.);             // how much we will shift the vertex from its on-line position

    if (fillDirection == 0.) {
        // we are stroking: need to calculate the miter or knee
        vec2 prev = dataScalingTransform * prevNext.xy;
        vec2 next = dataScalingTransform * prevNext.zw;
        vec2 lineToward = normalize(posPx - prev);
        vec2 lineAway = normalize(next - posPx);
        vec2 normal = vec2(lineAway.y, -lineAway.x);
        vec2 averageTangent = (prev == posPx) ? lineAway : normalize(lineToward + lineAway);
        vec2 prevNext = next - prev;
        vec2 miter = normalize(vec2(-averageTangent.y, averageTangent.x));
        float halfLineWidth = lineWidth / 2.0;
        float miterLength = halfLineWidth / dot(normal, miter);

        if (dot(lineToward, lineAway) >= 0) {
            // angle is right or obtuse: OK to use ordinary miter
            offset = -t * miterLength * miter;
        } else {
            // angle is acute: make a beveled miter to avoid overshooting too far
            vec2 upToCap = miter * halfLineWidth * t;
            vec2 capDeviation = averageTangent * halfLineWidth;
            if (lineToward.y > 0) {
                capDeviation *= sign(i - 2.0) * mod(i - 2.0, 2.0); // lower knee
                offset = oddMult * (upToCap + capDeviation) + evenMult * (-t * miterLength * miter);
            } else {
                capDeviation *= sign(i - 1.0) * mod(i - 1.0, 2.0); // upper knee
                offset = evenMult * (upToCap + capDeviation) + oddMult * (-t * miterLength * miter);
            }
        }
    }

    posPx.x += dataTransform[3][0];
    posPx.y += dataTransform[3][1]; // y offset, between 0 and the height of the graph

    if (fillDirection != 0.) {
        // we are filling space above or below the line
        // odd vertices stay in place, while even vertices are moved to the top or bottom
        posPx.y *= oddMult;
        // TODO tweak offset.y if fillDirection > 0
    }

    // apply the calculated offset to the vertex, and then apply Qt's usual
    // transform to go from Item coordinates to Window coordinates (and in 3D)
    gl_Position = qt_Matrix * vec4(posPx + offset, 0, 1.0);

    vT = t * 0.5;
    color = pos.y > warningAboveMaximum ? warningMaxColor :
            pos.y < warningBelowMinimum ? warningMinColor : normalColor;
}

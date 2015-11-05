#version 120
attribute highp vec4 pos; // x, y, i, t
attribute highp vec4 prevNext;

uniform lowp float lineWidth;
uniform lowp float alertBelowMinimum;
uniform lowp float alertAboveMaximum;
uniform lowp vec4 normalColor;
uniform lowp vec4 alertMinColor;
uniform lowp vec4 alertMaxColor;
uniform highp mat4 dataTransform;
uniform highp mat4 qt_Matrix;

varying lowp float vT;
varying lowp vec4 color;

void main(void)
{
    float i = pos.z;
    float t = pos.w;
    mat2 dataScalingTransform = mat2(dataTransform);
    vec2 prev = dataScalingTransform * prevNext.xy;
    vec2 next = dataScalingTransform * prevNext.zw;
    vec2 posPx = dataScalingTransform * pos.xy;
    vec2 yOffset = vec2(dataTransform[3][0], dataTransform[3][1]);
    vec2 lineToward = normalize(posPx - prev);
    vec2 lineAway = normalize(next - posPx);
    vec2 normal = vec2(lineAway.y, -lineAway.x);
    vec2 averageTangent = (prev == posPx) ? lineAway : normalize(lineToward + lineAway);
    vec2 prevNext = next - prev;
    vec2 miter = normalize(vec2(-averageTangent.y, averageTangent.x));
    float halfLineWidth = lineWidth / 2.0;
    float miterLength = halfLineWidth / dot(normal, miter);

    vec2 miterOff;
    if (dot(lineToward, lineAway) >= 0) { // angle is right or obtuse: OK to use ordinary miter
        miterOff = -t * miterLength * miter;
    } else { // angle is acute: make a knee
        vec2 upToCap = miter * sign(lineToward.y) * lineWidth * t;
        vec2 capDeviation = averageTangent * halfLineWidth * (averageTangent.x > 0 ? sign(i - 2.0) * mod(i - 2.0, 2.0) : sign(i - 2.0) * -mod(i - 1.0, 2.0));
//        capDeviation = averageTangent * halfLineWidth * i;

        miterOff = upToCap + capDeviation;
    }
    gl_Position = qt_Matrix * vec4(posPx + yOffset + miterOff, 0, 1.0);

//    gl_Position = qt_Matrix * vec4(posPx + yOffset + t * miterLength * miter, 0.0, 1.0); // old unlimited mitering

    vT = t * 0.5;
    color = pos.y > alertAboveMaximum ? alertMaxColor :
            pos.y < alertBelowMinimum ? alertMinColor : normalColor;
}

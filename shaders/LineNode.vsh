#version 120
attribute highp vec2 pos;
attribute highp float t;
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
    mat2 dataScalingTransform = mat2(dataTransform);
    vec2 prev = dataScalingTransform * prevNext.xy;
    vec2 next = dataScalingTransform * prevNext.zw;
    vec2 posPx = dataScalingTransform * pos;
    vec2 yOffset = vec2(dataTransform[3][0], dataTransform[3][1]);
    vec2 line = normalize(next - posPx);
    vec2 normal = vec2(line.y, -line.x);
    vec2 averageTangent = (prev == posPx) ? line : normalize(posPx - prev) + line;
    vec2 miter = vec2(-averageTangent.y, averageTangent.x);
    float miterLength = lineWidth / dot(normal, miter);

    gl_Position = qt_Matrix * vec4(posPx + yOffset + t * miterLength * miter, 0.0, 1.0);
    vT = t;
    color = pos.y > alertAboveMaximum ? alertMaxColor :
            pos.y < alertBelowMinimum ? alertMinColor : normalColor;
}

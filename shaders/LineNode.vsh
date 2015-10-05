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
    vec2 prev = prevNext.xy;
    vec2 next = prevNext.zw;
    vec2 line = normalize(next - pos);
    float aspect = dataTransform[1][1] / dataTransform[0][0];
    vec2 normal = normalize(vec2(line.y * aspect, line.x));
    vec2 tangent1 = (prev == pos) ? line : normalize(normalize(pos - prev) + line);
//    mat2 miterTransform = mat2(23.3333, 0., 0., -100.0);
    vec2 miter = vec2(-tangent1.y, tangent1.x);
    float miterLength = lineWidth / dot(normal, miter);

    gl_Position = qt_Matrix * dataTransform * vec4(pos + t * miterLength * miter, 0.0, 1.0);
    vT = t;
    color = pos.y > alertAboveMaximum ? alertMaxColor :
            pos.y < alertBelowMinimum ? alertMinColor : normalColor;
}

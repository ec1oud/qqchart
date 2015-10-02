uniform lowp float qt_Opacity;
uniform lowp float spread;  // used to control whether we anti-alias or not
                            // which wasn't the original purpose...

varying lowp float vT;
varying lowp vec4 color;

#define PI 3.14159265359

void main(void)
{
    lowp float tt = smoothstep(0.0, spread, sin(vT * PI));

    gl_FragColor = color * qt_Opacity * tt;
}

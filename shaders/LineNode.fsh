uniform lowp float qt_Opacity;
uniform lowp float aa;

varying lowp float vT;
varying lowp vec4 vColor;

#define PI 3.14159265359

void main(void)
{
    lowp float tt = abs(aa - 1.0) + aa * cos(vT * PI);

    gl_FragColor = vColor * qt_Opacity * tt;
}

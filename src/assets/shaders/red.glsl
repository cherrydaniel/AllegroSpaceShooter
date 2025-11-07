#ifdef GL_ES
precision mediump float;
#endif

uniform sampler2D al_tex;
varying vec4 varying_color;
varying vec2 varying_texcoord;

void main()
{
    vec4 t = varying_color*texture2D(al_tex, varying_texcoord);
    if (t==vec4(.0))
    {
        gl_FragColor = t;
        return;
    }
    t.r *= 6.;
    t.g *= .5;
    t.b *= .5;
    gl_FragColor = vec4(t);
}

#version 120

uniform float fade_factor;
uniform sampler2D textures[2];

varying vec2 texcoord;

void main()
{
    gl_FragColor = vec4(0.0f, 1.0f, 0.0f, 0.0f);
}

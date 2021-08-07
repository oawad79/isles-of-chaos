#version 330

in vec2 fragTexCoord;
in vec4 fragColor;

uniform vec2 textureSize = vec2(256, 144);
uniform sampler2D texture0;
uniform vec4 colDiffuse;

uniform float time;

out vec4 finalColor;

#define NIGHT vec4(0.1, 0.15, 0.4, 1.0)

vec4 lerp_v4(vec4 a, vec4 b, float t) {
    return a + (b - a) * t;
}

void main() {
    vec4 spriteColor = texture(texture0, fragTexCoord);
    finalColor = spriteColor;
    finalColor *= lerp_v4(vec4(1,1,1,1), NIGHT, (time + 1.0) * 0.5);
}

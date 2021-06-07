#version 330

in vec2 fragTexCoord;
in vec4 fragColor;

uniform vec2 textureSize = vec2(256, 144);
uniform sampler2D texture0;
uniform vec4 colDiffuse;

out vec4 finalColor;

void main() {
    vec4 spriteColor = texture(texture0, fragTexCoord);
    finalColor = spriteColor;
}

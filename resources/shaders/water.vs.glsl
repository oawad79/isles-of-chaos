#version 330

// Input vertex attributes
in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec3 vertexNormal;
in vec4 vertexColor;

// Input uniform values
uniform mat4 mvp;
uniform float timer = 0.0f;

// Output vertex attributes (to fragment shader)
out vec2 fragTexCoord;
out vec4 fragColor;
out vec3 fsVertexPosition;
out float fsTimer;

void main() {
    // Send vertex attributes to fragment shader
    fragTexCoord = vertexTexCoord;
    fragColor = vertexColor;
    fsTimer = timer;

    // Calculate final vertex position
    gl_Position = mvp*vec4(vertexPosition, 1.0);

    gl_Position.y += cos((timer*3) + vertexPosition.x * 1.5) * 0.03;
    gl_Position.y += cos((timer*5) + vertexPosition.x * 1.2) * 0.015;
    gl_Position.y += cos((timer*8) + vertexPosition.x * 1.0) * 0.005;

    fsVertexPosition = vertexPosition;
}

#version 430

in vec3 vNormal;

layout(location = 0) out vec4 gAlbedo;
layout(location = 1) out vec4 gNormal;
layout(location = 2) out vec4 gSpecular;

uniform vec3 fluidColor;

void main() {
    gAlbedo = vec4(fluidColor, 1.0);
    gNormal = vec4(normalize(vNormal), 1.0);
    gSpecular = vec4(0.0, 0.0, 0.0, 1.0);
}
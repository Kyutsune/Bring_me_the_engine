#version 410 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;

layout (location = 2) in vec3 aColor; // Optionnel pour le moment
layout(location = 3) in vec2 aUV;

layout(location = 4) in vec3 aTangent;
layout(location = 5) in vec3 aBitangent;

uniform mat4 mvpMatrix;
uniform mat4 modelMatrix;
uniform mat4 normalMatrix;

out vec3 vWorldPos;
out vec3 vNormal;
out vec2 vUV;
out vec3 vTangent;
out vec3 vBitangent;

void main()
{
    vec4 worldPos = modelMatrix * vec4(aPosition, 1.0);
    vWorldPos = worldPos.xyz;
    vUV = aUV;

    mat3 normMat = mat3(normalMatrix);

    vNormal    = normalize(normMat * aNormal);
    vTangent   = normalize(normMat * aTangent);
    vBitangent = normalize(normMat * aBitangent);

    gl_Position = mvpMatrix * vec4(aPosition, 1.0);
}

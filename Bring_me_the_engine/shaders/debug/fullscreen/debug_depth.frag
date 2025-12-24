#version 410 core

in vec2 uv;
out vec4 FragColor;

uniform sampler2D depthTexture;
uniform sampler2D albedoTexture;
uniform sampler2D normalTexture;
uniform sampler2D specularTexture;

uniform int debugMode; 
// 0 = depth
// 1 = albedo
// 2 = normal
// 3 = specular

uniform float nearPlane;
uniform float farPlane;

float linearizeDepth(float d)
{
    float z = d * 2.0 - 1.0;
    return (2.0 * nearPlane * farPlane) /
           (farPlane + nearPlane - z * (farPlane - nearPlane));
}

void main()
{
    if (debugMode == 0) {
        float depth = texture(depthTexture, uv).r;
        float linearDepth = linearizeDepth(depth) / farPlane;
        FragColor = vec4(vec3(linearDepth), 1.0);
    }
    else if (debugMode == 1) {
        FragColor = texture(albedoTexture, uv);
    }
    else if (debugMode == 2) {
        vec3 n = texture(normalTexture, uv).rgb;
        FragColor = vec4(n, 1.0);
    }
    else if (debugMode == 3) {
        vec3 s = texture(specularTexture, uv).rgb;
        FragColor = vec4(s, 1.0);
    }
}

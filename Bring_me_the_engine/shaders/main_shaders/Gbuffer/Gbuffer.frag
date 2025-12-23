#version 450 core

layout(location = 0) out vec4 gAlbedo;
layout(location = 1) out vec3 gNormal;

in vec3 vWorldPos;
in vec3 vNormal;
in vec2 vUV;

uniform sampler2D albedoMap;
uniform vec3 baseColor;
uniform int useTexture; // 1 = texture, 0 = couleur simple

void main()
{
    vec3 albedo = baseColor;
    if(useTexture == 1)
        albedo = texture(albedoMap, vUV).rgb;

    gAlbedo = vec4(albedo, 1.0);
    gNormal = normalize(vNormal) * 0.5 + 0.5;
}

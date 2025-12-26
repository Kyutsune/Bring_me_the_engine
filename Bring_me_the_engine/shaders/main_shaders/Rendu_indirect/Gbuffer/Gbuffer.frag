#version 410 core

layout(location = 0) out vec4 gAlbedo;
layout(location = 1) out vec3 gNormal;
layout(location = 2) out vec3 gSpecular;

in vec3 vWorldPos;
in vec3 vNormal;
in vec2 vUV;
in vec3 vTangent;
in vec3 vBitangent;

uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D specularMap;
uniform bool useNormalMap;
uniform bool useSpecularMap;


uniform vec3 baseColor;
uniform int useTexture; // 1 = texture, 0 = couleur simple

vec3 getNormal() {
    if (!useNormalMap) return normalize(vNormal);

    vec3 T = normalize(vTangent);
    vec3 N = normalize(vNormal);
    vec3 B = normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    vec3 normalMap = texture(normalMap, vUV).rgb * 2.0 - 1.0;
    return normalize(TBN * normalMap);
}

void main()
{
    vec4 albedo = vec4(baseColor, 1.0);
    if(useTexture == 1){
        albedo = texture(albedoMap, vUV);
        if(albedo.a < 0.1) {
            discard;
        }
    }
    gAlbedo = vec4(albedo);

    vec3 normal = getNormal();
    gNormal = gl_FrontFacing ? normal : -normal;



    vec3 spec = vec3(1.0);
    if (useSpecularMap)
        spec = texture(specularMap, vUV).rgb;
    gSpecular = spec;
}

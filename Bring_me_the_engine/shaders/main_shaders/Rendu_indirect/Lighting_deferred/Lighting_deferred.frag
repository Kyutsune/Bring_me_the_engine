#version 410 core

out vec4 FragColor;
in vec2 TexCoord;

// --- Les buffers qui définissent le gbuffer ---
uniform sampler2D gDepth;
uniform sampler2D gAlbedo;
uniform sampler2D gNormal;
uniform sampler2D gSpecular;


// --- Reconstruction Position ---
uniform mat4 inverseProjection;
uniform mat4 inverseView;

// --- Informations concernant les lumières ---
#define MAX_LIGHTS 8
#define MAX_POINT_LIGHTS 8

struct Light {
    int type;        // 0 = ponctuelle, 1 = directionnelle
    vec3 position;
    vec3 direction;
    vec3 color;
    float intensity;
    float constant;
    float linear;
    float quadratic;
};

uniform int numLights;
uniform Light lights[MAX_LIGHTS];

uniform vec3 ambientColor;
uniform float ambientStrength;
uniform vec3 diffuseColor;
uniform float diffuseIntensity;
uniform vec3 specularColor;
uniform float specularStrength;
uniform float shininess;

uniform vec3 viewPos;

// --- Fog ---
uniform vec3 fogColor;
uniform float fogStart;    
uniform float fogEnd;
uniform float fogDensity;  
uniform int fogType;       

// --- Shadows ---
uniform bool useDirectionalShadow;
uniform sampler2D shadowMap;
uniform mat4 lightSpaceMatrix; // Nécessaire pour calculer FragPosLightSpace
uniform vec3 dirLightDirection;

uniform bool usePointShadow;
uniform int pointLightNumber;
uniform samplerCube pointShadowMaps[MAX_POINT_LIGHTS];
uniform vec3 pointLightPositions[MAX_POINT_LIGHTS];
uniform float pointLightFarPlanes[MAX_POINT_LIGHTS];
uniform float pointLightIntensities[MAX_POINT_LIGHTS];

// Reconstruit la position Monde à partir du Depth Buffer
vec3 WorldPosFromDepth(vec2 uv, float depth) {
    vec4 clipPos = vec4(uv * 2.0 - 1.0, depth * 2.0 - 1.0, 1.0);
    
    vec4 viewPos = inverseProjection * clipPos;
    viewPos /= viewPos.w;
    
    vec4 worldPos = inverseView * viewPos;
    return worldPos.xyz;
}

float getFogFactor(float dist) {
    if (fogType == 1) return clamp((fogEnd - dist) / (fogEnd - fogStart), 0.0, 1.0);
    else if (fogType == 2) return exp(-fogDensity * dist);
    else if (fogType == 3) return exp(-pow(fogDensity * dist, 2.0));
    else return 1.0;
}

vec3 calcAmbient(vec3 norm) {
    vec3 upColor = vec3(1.0, 1.0, 1.0);
    vec3 downColor = vec3(0.3, 0.3, 0.3);
    float factor = norm.y * 0.5 + 0.5;
    return mix(downColor, upColor, factor) * ambientColor * ambientStrength;
}

// Ombres directionnelles
float calculateDirShadow(vec3 worldPos, vec3 normal) {
    if (!useDirectionalShadow) return 1.0;

    // On projette la position monde dans l'espace lumière ici
    vec4 fragPosLightSpace = lightSpaceMatrix * vec4(worldPos, 1.0);
    
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    
    if (projCoords.z > 1.0 || projCoords.x < 0.0 || projCoords.x > 1.0 || 
        projCoords.y < 0.0 || projCoords.y > 1.0) return 1.0;

    float currentDepth = projCoords.z;
    vec3 lightDir = normalize(dirLightDirection);
    float bias = max(0.005 * (1.0 - dot(normal, lightDir)), 0.001);

    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x) {
        for(int y = -1; y <= 1; ++y) {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    return clamp(1.0 - shadow, 0.0, 1.0);
}

// Offsets pour les ombres ponctuelles
const vec3 sampleOffsetDirections[20] = vec3[](
   vec3( 1,  1,  1), vec3( -1,  1,  1), vec3( 1, -1,  1), vec3( -1, -1,  1),
   vec3( 1,  1, -1), vec3( -1,  1, -1), vec3( 1, -1, -1), vec3( -1, -1, -1),
   vec3( 1,  0,  0), vec3( -1,  0,  0), vec3( 0,  1,  0), vec3( 0, -1,  0),
   vec3( 0,  0,  1), vec3( 0,  0, -1), vec3( 1,  1,  0), vec3( -1,  1,  0),
   vec3( 1, -1,  0), vec3( -1, -1,  0), vec3( 0,  1,  1), vec3( 0, -1, -1)
);

// Ombres ponctuelles
float calculatePointShadow(vec3 fragPos, vec3 normal, int lightIndex) {
    if (lightIndex < 0 || lightIndex > pointLightNumber) return 1.0;

    vec3 lightToFrag = fragPos - pointLightPositions[lightIndex];
    float currentDepth = length(lightToFrag);

    float shadow = 0.0;
    vec3 lightDir = normalize(lightToFrag);
    float bias = max(0.05 * (1.0 - dot(normal, -lightDir)), 0.005);
    int samples = 20;
    float viewDistance = length(viewPos - fragPos);
    float diskRadius = 0.05 * (viewDistance / pointLightFarPlanes[lightIndex]);

    for (int i = 0; i < samples; ++i) {
        vec3 sampleDir = normalize(lightToFrag + sampleOffsetDirections[i] * diskRadius);
        float closestDepth = texture(pointShadowMaps[lightIndex], sampleDir).r * pointLightFarPlanes[lightIndex];
        float diff = currentDepth - bias - closestDepth;
        float weight = 1.0 - length(sampleOffsetDirections[i]) / sqrt(5.0);
        shadow += weight * smoothstep(0.0, 0.10, diff);
    }
    shadow /= float(samples);
    float attenuationFactor = exp(-currentDepth / pointLightFarPlanes[lightIndex]);
    shadow *= attenuationFactor;
    shadow = pow(shadow, 0.3); 

    return mix(0.2, 1.0, 1.0 - clamp(shadow, 0.0, 1.0)) * pointLightIntensities[lightIndex];
}

// Calcul d'éclairage unique
vec3 calcLight(Light light, vec3 norm, vec3 viewDir, vec3 fragPos, float shadowFactor, vec3 specMapVal) {
    vec3 lightDir;
    float attenuation = 1.0;
    float lightIntensity = light.intensity;

    if (light.type == 0) { // Ponctuelle
        lightDir = normalize(light.position - fragPos);
        float distance = length(light.position - fragPos);
        attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    } else if (light.type == 1) { // Directionnelle
        lightDir = normalize(-light.direction);
        attenuation = 1.0;
        lightIntensity *= 0.1; // Ton facteur arbitraire du forward
    } else {
        lightDir = vec3(0.0);
        attenuation = 1.0;
    }

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = attenuation * diff * diffuseIntensity * diffuseColor * light.color * lightIntensity;

    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = attenuation * specularStrength * specMapVal * spec * specularColor * light.color * lightIntensity;

    if (light.type == 1) {
        diffuse *= shadowFactor;
        specular *= shadowFactor;
    }

    return diffuse + specular;
}

void main()
{
    // Récupération des données du G-Buffer
    float depth = texture(gDepth, TexCoord).r;


    if(depth >= 1.0) {
        discard;
    }
    
    vec3 FragPos = WorldPosFromDepth(TexCoord, depth);

    vec3 Albedo = texture(gAlbedo, TexCoord).rgb;
    // Décodage de la normale : [0,1] -> [-1,1]
    vec3 Normal = normalize(texture(gNormal, TexCoord).rgb);
    vec3 SpecularData = texture(gSpecular, TexCoord).rgb;

    // Conversion SpecularData en float pour le lighting 
    float specMapVal = dot(SpecularData, vec3(0.299, 0.587, 0.114));

    //  Calculs préliminaires
    vec3 viewDir = normalize(viewPos - FragPos);
    float distance_to_obj = length(viewPos - FragPos);
    float fogFactor = getFogFactor(distance_to_obj);

    // Calcul de l'éclairage
    vec3 result = calcAmbient(Normal);
    float shadowFactor = calculateDirShadow(FragPos, Normal);

    int pointLightShadowIndex = 0;

    for (int i = 0; i < numLights; i++) {
        vec3 lighting = calcLight(lights[i], Normal, viewDir, FragPos, shadowFactor, vec3(specMapVal));

        if (lights[i].type == 0) { 
            if (usePointShadow && pointLightShadowIndex < pointLightNumber) {
                float shadowVal = calculatePointShadow(FragPos, Normal, pointLightShadowIndex);
                lighting *= shadowVal;
                pointLightShadowIndex++;
            }
        }
        result += lighting;
    }

    result *= Albedo;
    result = clamp(result, 0.0, 1.0);

    vec3 finalColor = mix(fogColor, result, fogFactor);
    
    FragColor = vec4(finalColor, 1.0);
}
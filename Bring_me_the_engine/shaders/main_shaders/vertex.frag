#version 410 core

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

// Touts les paramètres relatifs à la lumière (systeme Phong pour le moment)
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

// Couleur de base du matériau (si pas de texture)
uniform vec3 baseColor;
uniform bool useVertexColor;

// Toutes les composantes de la texture (diffuse, normal, spéculaire)
uniform sampler2D texture_diffuse;
uniform sampler2D texture_normal;
uniform sampler2D texture_specular;

uniform bool useTexture;
uniform bool useNormalMap;
uniform bool useSpecularMap;

// Les informations sur le fog 
uniform vec3 fogColor;
uniform float fogStart;    
uniform float fogEnd;
uniform float fogDensity;  
uniform int fogType;       // 0: aucun, 1: linéaire, 2: exp, 3: exp²

// Les informations sur le pixel cible
in vec3 FragPos;
in vec3 Normal;
in vec3 vColor;
in vec2 TexCoord;
in vec3 Tangent;
in vec3 Bitangent; // Pas utilisée

out vec4 FragColor;

// Les informations pour ajouter des ombres sur les lumières directionnelles
uniform bool useDirectionalShadow;
uniform sampler2D shadowMap;
in vec4 FragPosLightSpace;
uniform vec3 dirLightDirection;

// Les informations pour ajouter des ombres sur les lumières ponctuelles
uniform bool usePointShadow;
uniform int pointLightNumber;
uniform samplerCube pointShadowMaps[MAX_POINT_LIGHTS];
uniform vec3 pointLightPositions[MAX_POINT_LIGHTS];
uniform float pointLightFarPlanes[MAX_POINT_LIGHTS];
uniform float pointLightIntensities[MAX_POINT_LIGHTS];


vec3 getNormal() {
    if (!useNormalMap) return normalize(Normal);

    vec3 T = normalize(Tangent);
    vec3 N = normalize(Normal);
    vec3 B = normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    vec3 normalMap = texture(texture_normal, TexCoord).rgb * 2.0 - 1.0;
    return normalize(TBN * normalMap);
}

float getFogFactor(float dist) {
    if (fogType == 1)
        return clamp((fogEnd - dist) / (fogEnd - fogStart), 0.0, 1.0);
    else if (fogType == 2)
        return exp(-fogDensity * dist);
    else if (fogType == 3)
        return exp(-pow(fogDensity * dist, 2.0));
    else
        return 1.0;
}

vec3 calcAmbient(vec3 norm) {
    vec3 upColor = vec3(1.0, 1.0, 1.0);
    vec3 downColor = vec3(0.3, 0.3, 0.3);
    float factor = norm.y * 0.5 + 0.5;
    return mix(downColor, upColor, factor) * ambientStrength;
}

vec3 calcLight(Light light, vec3 norm, vec3 viewDir, vec3 fragPos, float shadowFactor) {
    vec3 lightDir;
    float attenuation = 1.0;
    float lightIntensity = light.intensity;

    if (light.type == 0) {
        lightDir = normalize(light.position - fragPos);
        float distance = length(light.position - fragPos);
        attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    } else if (light.type == 1) {
        lightDir = normalize(-light.direction);
        attenuation = 1.0;
        lightIntensity *= 0.1;
    } else {
        lightDir = vec3(0.0);
        attenuation = 1.0;
    }

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = attenuation * diff * diffuseIntensity * diffuseColor * light.color * lightIntensity;

    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);

    float specMapVal = useSpecularMap ? dot(texture(texture_specular, TexCoord).rgb, vec3(0.299, 0.587, 0.114)) : 1.0;
    vec3 specular = attenuation * specularStrength * specMapVal * spec * specularColor * light.color * lightIntensity;

    // Appliquer l'ombre seulement aux composantes diffuse et spéculaire
    if (light.type == 1) { // Lumière directionnelle seulement
        diffuse *= shadowFactor;
        specular *= shadowFactor;
    }

    return diffuse + specular;
}

float calculateDirShadow(vec4 fragPosLightSpace) {
    if (!useDirectionalShadow)
        return 1.0; // Pas d'ombre si on a pas dans la scène de lumière directionnelle

    // Perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    
    // Transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    
    // Si on est en dehors de la shadow map, on considère qu'il n'y a pas d'ombre
    if (projCoords.z > 1.0 || projCoords.x < 0.0 || projCoords.x > 1.0 || 
        projCoords.y < 0.0 || projCoords.y > 1.0) {
        return 1.0; // Pas d'ombre
    }

    // Get closest depth value from light's perspective
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    
    // Get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;

    // Calculate bias (fixes shadow acne)
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(dirLightDirection);
    float bias = max(0.005 * (1.0 - dot(normal, lightDir)), 0.001);

    // Check whether current frag pos is in shadow
    // PCF (Percentage-closer filtering) pour des ombres plus douces
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


const vec3 sampleOffsetDirections[20] = vec3[](
    vec3( 1,  1,  1), vec3( -1,  1,  1), vec3( 1, -1,  1), vec3( -1, -1,  1),
    vec3( 1,  1, -1), vec3( -1,  1, -1), vec3( 1, -1, -1), vec3( -1, -1, -1),
    vec3( 1,  0,  0), vec3( -1,  0,  0), vec3( 0,  1,  0), vec3( 0, -1,  0),
    vec3( 0,  0,  1), vec3( 0,  0, -1), vec3( 1,  1,  0), vec3( -1,  1,  0),
    vec3( 1, -1,  0), vec3( -1, -1,  0), vec3( 0,  1,  1), vec3( 0, -1, -1)
);

float calculatePointShadow(vec3 fragPos, int lightIndex) {
    if (lightIndex < 0 || lightIndex > pointLightNumber) {
        return 1.0; // Pas d'ombre hors index valide
    }

    vec3 lightToFrag = fragPos - pointLightPositions[lightIndex];
    float currentDepth = length(lightToFrag);

    float shadow = 0.0;
    vec3 normal = normalize(Normal);
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

    // Atténuation avec la distance
    float attenuationFactor = exp(-currentDepth / pointLightFarPlanes[lightIndex]);
    shadow *= attenuationFactor;

    // Renforcer les ombres proches avec une courbe non linéaire
    shadow = pow(shadow, 0.3); 

    return mix(0.2, 1.0, 1.0 - clamp(shadow, 0.0, 1.0)) * pointLightIntensities[lightIndex];
}


void main() {
    vec3 norm = getNormal();
    vec3 viewDir = normalize(viewPos - FragPos);

    float distance_to_obj = length(viewPos - FragPos);
    float fogFactor = getFogFactor(distance_to_obj);

    vec3 result = calcAmbient(norm);

    // Calculer le facteur d'ombre une seule fois
    float shadowFactor = calculateDirShadow(FragPosLightSpace);

    float pointShadow = usePointShadow ? calculatePointShadow(FragPos, 0) : 1.0;


        int pointLightShadowIndex = 0;

        for (int i = 0; i < numLights; i++) {
            vec3 lighting = calcLight(lights[i], norm, viewDir, FragPos, shadowFactor);

            if (lights[i].type == 0) { // Si c'est une lumière ponctuelle
                if (usePointShadow && pointLightShadowIndex < pointLightNumber) {
                    float shadowVal = calculatePointShadow(FragPos, pointLightShadowIndex);
                    lighting *= shadowVal;
                    pointLightShadowIndex++;
                }
            }

            result += lighting;
        }

    vec3 surfaceColor;

    if (useTexture) {
        // Cas texture diffuse
        vec4 texurer = texture(texture_diffuse, TexCoord);
        if(texurer.a < 0.1) discard;
        surfaceColor = texture(texture_diffuse, TexCoord).rgb;
    } else {
        // Cas sans texture
        if (useVertexColor)
            surfaceColor = vColor * baseColor; // combine les deux
        else
            surfaceColor = baseColor; // couleur uniforme du matériau
    }

    result *= surfaceColor;
    result = clamp(result, 0.0, 1.0);

    vec3 finalColor = mix(fogColor, result, fogFactor);
    FragColor = vec4(finalColor, 1.0);
}
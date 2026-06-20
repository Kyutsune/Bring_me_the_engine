#version 430

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;

struct Particle {
    vec4 position; // xyz: position, w: densité
    vec4 velocity; // xyz: vitesse,  w: pression
};

layout(std430, binding = 0) buffer InBuffer {
    Particle particles[];
} inBuf;

uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform float particleRadius;

out vec3 vNormal;

void main() {
    // Récupération de la position de la particule courante via l'ID de l'instance
    vec3 particlePos = inBuf.particles[gl_InstanceID].position.xyz;

    // On applique le rayon de la particule et on la décale dans le monde
    vec3 worldPos = (aPos * particleRadius) + particlePos;

    vNormal = aNormal; // Pas besoin de matrice model complexe pour une sphère uniforme

    gl_Position = projectionMatrix * viewMatrix * vec4(worldPos, 1.0);
}
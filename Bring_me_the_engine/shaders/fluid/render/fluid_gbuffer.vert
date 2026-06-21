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

uniform bool isObstacleMode;
uniform vec3 obstaclePos;
uniform float obstacleRadius;

out vec3 vNormal;

void main() {
    vec3 worldPos;
    if (isObstacleMode) {
        // Mode Obstacle : On utilise les coordonnées de la sphère fixe
        worldPos = (aPos * obstacleRadius) + obstaclePos;
    } else {
        // Mode Fluide : Récupération classique via l'ID de l'instance [cite: 40]
        vec3 particlePos = inBuf.particles[gl_InstanceID].position.xyz;
        worldPos = (aPos * particleRadius) + particlePos;
    }

    vNormal = aNormal;
    
    gl_Position = projectionMatrix * viewMatrix * vec4(worldPos, 1.0);
}

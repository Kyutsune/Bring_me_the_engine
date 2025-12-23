#version 450 core

in vec2 uv;
out vec4 FragColor;

uniform sampler2D depthTexture;
uniform float nearPlane;
uniform float farPlane;

// Linearisation de la depth
float linearizeDepth(float d)
{
    float z = d * 2.0 - 1.0;
    return (2.0 * nearPlane * farPlane) /
           (farPlane + nearPlane - z * (farPlane - nearPlane));
}

void main()
{
    float depth = texture(depthTexture, uv).r;
    float linearDepth = linearizeDepth(depth) / farPlane;

    // Bleu = profondeur
    FragColor = vec4(0.0, 0.0, linearDepth, 1.0);
}

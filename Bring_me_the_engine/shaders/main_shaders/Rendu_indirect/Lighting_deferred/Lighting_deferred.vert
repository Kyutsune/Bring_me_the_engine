#version 410 core

out vec2 TexCoord;

void main()
{

    // Définition du triangle géant qui couvre l'écran
    vec2 positions[3] = vec2[3](
        vec2(-1.0, -1.0), // Bas-Gauche
        vec2( 3.0, -1.0), // Bas-Droite (loin)
        vec2(-1.0,  3.0)  // Haut-Gauche (loin)
    );


    TexCoord = (positions[gl_VertexID] + 1.0) * 0.5;
    gl_Position = vec4(positions[gl_VertexID], 0.0, 1.0);
}
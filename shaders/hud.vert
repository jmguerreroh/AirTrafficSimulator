#version 330 core
// hud.vert - Panel de información en pantalla (2D). Model ya está en píxeles de pantalla.

layout(location = 0) in vec3 aPosition;
layout(location = 2) in mat4 aModel;
layout(location = 6) in vec4 aColor;

uniform mat4 uProjection;  // ortográfica: (0,0) arriba a la izquierda, unidades = píxeles

out vec4 vColor;

void main()
{
    vColor = aColor;
    gl_Position = uProjection * aModel * vec4(aPosition, 1.0);
}

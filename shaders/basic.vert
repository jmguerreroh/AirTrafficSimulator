#version 330 core
// basic.vert - Vertex shader de la escena 3D.
//
// Cadena de transformaciones:  clip = Projection * View * Model * posición
//   Model      (por instancia)  coloca/orienta/escala cada objeto en el mundo.
//   View       (uniform)        mueve el mundo para verlo desde la cámara.
//   Projection (uniform)        aplica la perspectiva.
// Model llega como atributo de instancia (locations 2..5): así todas las piezas de un mismo tipo
// (casillas, cubos de los modelos, píxeles de las etiquetas...) se dibujan con UNA llamada.

layout(location = 0) in vec3 aPosition;  // posición del vértice (espacio del objeto)
layout(location = 1) in vec3 aNormal;    // normal del vértice
layout(location = 2) in mat4 aModel;     // ocupa las locations 2, 3, 4 y 5
layout(location = 6) in vec4 aColor;     // rgb = color; a = emisión (0 = iluminado, 1 = color plano brillante)

uniform mat4 uView;
uniform mat4 uProjection;

out vec3 vWorldPos;
out vec3 vNormal;
out vec4 vColor;

void main()
{
    vec4 world = aModel * vec4(aPosition, 1.0);
    vWorldPos = world.xyz;
    // La matriz normal (inversa traspuesta) mantiene las normales correctas con escalados no uniformes.
    vNormal = transpose(inverse(mat3(aModel))) * aNormal;
    vColor = aColor;
    gl_Position = uProjection * uView * world;
}

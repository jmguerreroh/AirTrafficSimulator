#version 330 core
// hud.frag - Color plano; el alfa es la opacidad (para el fondo del panel).

in vec4 vColor;
out vec4 FragColor;

void main()
{
    FragColor = vColor;
}

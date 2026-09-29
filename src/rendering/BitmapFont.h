// BitmapFont.h - Fuente 5x7 incorporada (sin ficheros ni bibliotecas de texto).
//
// El texto se convierte en una lista de "píxeles" encendidos; el Renderer dibuja cada uno como un
// cubito (en 3D para las etiquetas, en pantalla para el panel de información).
// Solo mayúsculas, dígitos y algunos signos; el resto se dibuja como '?'.
#pragma once

#include <string>
#include <vector>

#include <glm/vec2.hpp>

struct TextPixels {
    std::vector<glm::ivec2> pixels;  // (columna, fila) con fila 0 arriba
    int width = 0;                   // en píxeles de fuente
    int height = 0;
};

constexpr int kGlyphAdvance = 6;  // 5 de ancho + 1 de separación
constexpr int kGlyphHeight = 7;

TextPixels rasterizeText(const std::string& text);

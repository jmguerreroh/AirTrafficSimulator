// Color.h - Color RGB (0..1) asociado a cada aeronave.
#pragma once

#include <cmath>
#include <cstddef>

struct Color {
    float r = 1.0f;
    float g = 1.0f;
    float b = 1.0f;
};

inline bool operator==(const Color& a, const Color& c) { return a.r == c.r && a.g == c.g && a.b == c.b; }
inline bool operator!=(const Color& a, const Color& c) { return !(a == c); }

// Paleta de colores bien distinguibles entre sí. Pasados los primeros 12 se generan
// tonos nuevos repartiendo el círculo cromático con la razón áurea.
inline Color colorPaleta(std::size_t i) {
    static const Color base[12] = {
        {0.90f, 0.15f, 0.15f},  // rojo
        {0.15f, 0.38f, 0.95f},  // azul
        {0.15f, 0.75f, 0.28f},  // verde
        {1.00f, 0.55f, 0.05f},  // naranja
        {0.98f, 0.88f, 0.10f},  // amarillo
        {0.60f, 0.25f, 0.85f},  // morado
        {0.10f, 0.80f, 0.85f},  // cian
        {0.95f, 0.20f, 0.65f},  // magenta
        {0.65f, 0.90f, 0.15f},  // lima
        {0.60f, 0.38f, 0.18f},  // marrón
        {1.00f, 0.62f, 0.72f},  // rosa
        {0.40f, 0.55f, 0.55f},  // verde azulado
    };
    if (i < 12) return base[i];
    const float h = std::fmod(static_cast<float>(i) * 0.618034f, 1.0f) * 6.0f;
    const float x = 1.0f - std::fabs(std::fmod(h, 2.0f) - 1.0f);
    const float c = 0.9f, m = 0.05f;
    float r = 0, g = 0, b = 0;
    switch (static_cast<int>(h)) {
        case 0: r = c; g = x; break;
        case 1: r = x; g = c; break;
        case 2: g = c; b = x; break;
        case 3: g = x; b = c; break;
        case 4: r = x; b = c; break;
        default: r = c; b = x; break;
    }
    return {r + m, g + m, b + m};
}

// Direccion.h - Las 8 direcciones de la rosa de los vientos y utilidades angulares.
//
// Convención de ángulos ("rumbo"): 0° = Norte (+Y), 90° = Este (+X), 180° = Sur, 270° = Oeste.
// Crecen en sentido horario visto desde arriba, como una brújula.
#pragma once

#include <cmath>

enum class Direccion { N = 0, NE, E, SE, S, SW, W, NW };

// Rumbo en grados de una dirección: N = 0, NE = 45, E = 90...
inline double gradosDe(Direccion d) { return 45.0 * static_cast<int>(d); }

// Lleva cualquier ángulo al intervalo [0, 360).
inline double normalizarGrados(double grados) {
    grados = std::fmod(grados, 360.0);
    return grados < 0.0 ? grados + 360.0 : grados;
}

// Diferencia angular con signo (destino - origen) en el intervalo (-180, 180].
// Positiva = hay que girar a la derecha (sentido horario).
inline double diferenciaAngular(double origen, double destino) {
    double d = normalizarGrados(destino - origen);
    return d > 180.0 ? d - 360.0 : d;
}

// Dirección de la rosa de los vientos más cercana a un rumbo en grados.
inline Direccion direccionDesdeGrados(double grados) {
    const int i = static_cast<int>(std::floor(normalizarGrados(grados) / 45.0 + 0.5)) % 8;
    return static_cast<Direccion>(i);
}

// Desplazamiento de una casilla en esa dirección (X: Este +, Y: Norte +).
inline int deltaX(Direccion d) {
    static const int t[8] = {0, 1, 1, 1, 0, -1, -1, -1};
    return t[static_cast<int>(d)];
}
inline int deltaY(Direccion d) {
    static const int t[8] = {1, 1, 0, -1, -1, -1, 0, 1};
    return t[static_cast<int>(d)];
}

inline bool esCardinal(Direccion d) { return static_cast<int>(d) % 2 == 0; }  // N, E, S, W

inline const char* aTexto(Direccion d) {
    static const char* const t[8] = {"N", "NE", "E", "SE", "S", "SW", "W", "NW"};
    return t[static_cast<int>(d)];
}

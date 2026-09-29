// Posicion.h - Celda del espacio de simulación.
//
// SISTEMA DE COORDENADAS
//   X  → Este  (izquierda → derecha)      0 ≤ x < tamX
//   Y  → Norte (hacia el fondo del mapa)  0 ≤ y < tamY
//   Z  → altura (hacia arriba)            0 ≤ z ≤ alturaMaximaTablero
// La celda (0, 0) es la esquina suroeste (abajo a la izquierda en la vista cenital inicial).
#pragma once

#include <ostream>

struct Posicion {
    int x = 0;
    int y = 0;
    int z = 0;
};

inline bool operator==(const Posicion& a, const Posicion& b) { return a.x == b.x && a.y == b.y && a.z == b.z; }
inline bool operator!=(const Posicion& a, const Posicion& b) { return !(a == b); }
inline std::ostream& operator<<(std::ostream& os, const Posicion& p) {
    return os << "(" << p.x << ", " << p.y << ", " << p.z << ")";
}

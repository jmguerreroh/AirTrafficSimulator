// Input.h - Entrada del usuario ya "digerida" por Window (así Camera y Renderer no dependen de GLFW).
#pragma once

#include <glm/vec2.hpp>

struct Input {
    glm::vec2 cursorDelta{0.0f};  // movimiento del ratón desde el frame anterior (píxeles)
    bool orbiting = false;        // botón izquierdo pulsado y arrastrando
    bool panning = false;         // botón derecho o central pulsado
    float scroll = 0.0f;          // pasos de rueda acumulados (+ = acercar)
    glm::vec2 keyMove{0.0f};      // teclado: x = derecha(+)/izquierda(-), y = adelante(+)/atrás(-)
    float keyRotate = 0.0f;       // teclado: +1 / -1 giro alrededor del tablero
    float keyZoom = 0.0f;         // teclado: +1 acercar / -1 alejar
    bool resetRequested = false;  // tecla R o 1: vista inicial
    bool topViewRequested = false;  // tecla 2: vista cenital
    bool clicked = false;         // clic izquierdo sin arrastrar (selección)
    glm::vec2 clickNdc{0.0f};     // posición del clic en coordenadas normalizadas [-1, 1] (Y hacia arriba)
};

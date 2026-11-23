// ATC.cpp - PLANTILLA PARA EL ALUMNO
// Aquí va tu sistema de control de tráfico aéreo.
#include "ATC.h"

void ATC::actualizar(std::vector<std::unique_ptr<Aeronave>>& aeronaves, double dt) {
    // TODO: IMPLEMENTAR POR EL ALUMNO
    //
    // Ejemplo del estilo esperado (recorrido polimórfico, sin saber qué tipo es cada una):
    //
    //     for (auto& aeronave : aeronaves) {
    //         if (!aeronave->estaActiva()) continue;
    //         // ... decidir: ¿cambiar la ruta? ¿frenar? ¿subir? ¿esperar? ...
    //     }
    //
    // Situaciones a resolver (ver docs/PRACTICA_ATC.md): evitar colisiones, gestionar alturas,
    // combustible, prioridades y emergencias.
    (void)aeronaves;
    (void)dt;
}

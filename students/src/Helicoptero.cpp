// Helicoptero.cpp - PLANTILLA PARA EL ALUMNO
//
// Las reglas del helicóptero están en include/Helicoptero.h. Completa los cuatro métodos.
// Herramientas (ver include/Aeronave.h): getModoControl(), tieneDestino(), getDestinoActual(),
// getX(), getY(), getVelocidad(), getRumbo(), fijarRumboGrados(), avanzar(rumbo, distancia),
// acercarAltura(objetivo, maxDelta), getAlturaObjetivo(), setModoControl(), esCardinal(dir)...
#include <algorithm>
#include <cmath>
#include <stdexcept>

#include "Helicoptero.h"

bool Helicoptero::puedeMoverse(Direccion direccion) const {
    // TODO: IMPLEMENTAR POR EL ALUMNO
    // Solo direcciones cardinales (N, E, S, W) y solo si la aeronave estaActiva().
    (void)direccion;
    return false;
}

void Helicoptero::mover(Direccion direccion) {
    // TODO: IMPLEMENTAR POR EL ALUMNO
    // Si no puedeMoverse(direccion): std::invalid_argument. Si sí: modo Manual y fijar el rumbo.
    (void)direccion;
}

double Helicoptero::calcularConsumo(double dt) const {
    // TODO: IMPLEMENTAR POR EL ALUMNO
    // Estacionario (getEstado() == Estacionaria): config_.consumoEstacionario * dt.
    // En movimiento: (consumoBase + consumoPorVelocidad * velocidad) * dt.
    (void)dt;
    return 0.0;
}

void Helicoptero::actualizar(double dt) {
    // TODO: IMPLEMENTAR POR EL ALUMNO
    // Mover el helicóptero durante dt segundos:
    //   * Modo Manual: avanzar en su rumbo getVelocidad() * dt.
    //   * Modo Ruta con destino: desplazarse SOLO en cardinales (no en diagonal): por ejemplo,
    //     primero alinearse en un eje y luego en el otro, sin pasarse del destino.
    //   * Sin destino: vuelo estacionario (no cambia de posición).
    //   * En todos los casos la altura puede cambiar sin desplazarse: acercarAltura(...).
    (void)dt;
}

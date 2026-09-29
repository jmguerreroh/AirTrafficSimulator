// Dron.cpp - PLANTILLA PARA EL ALUMNO
//
// Las reglas del dron están en include/Dron.h. Completa los cuatro métodos.
// Herramientas (ver include/Aeronave.h): gradosHacia(punto), distanciaHorizontalA(punto),
// fijarRumboGrados(), avanzar(rumbo, distancia), acercarAltura(objetivo, maxDelta),
// getAlturaObjetivo(), getModoControl(), tieneDestino(), getDestinoActual()...
// Y mantenerEspera(dt), que ya orbita alrededor de getCentroEspera().
#include <stdexcept>

#include "Dron.h"

bool Dron::puedeMoverse(Direccion direccion) const {
    (void)direccion;
    // TODO: IMPLEMENTAR POR EL ALUMNO
    // El dron puede tomar cualquier dirección (incluidas las diagonales) mientras estaActiva().
    return false;
}

void Dron::mover(Direccion direccion) {
    (void)direccion;
    // TODO: IMPLEMENTAR POR EL ALUMNO
    // Si no puedeMoverse(direccion): std::invalid_argument. Si sí: modo Manual y fijar el rumbo.
}

double Dron::calcularConsumo(double dt) const {
    (void)dt;
    // TODO: IMPLEMENTAR POR EL ALUMNO
    // Estacionario: config_.consumoEstacionario * dt. En movimiento: (base + porVelocidad * v) * dt.
    return 0.0;
}

void Dron::actualizar(double dt) {
    (void)dt;
    // TODO: IMPLEMENTAR POR EL ALUMNO
    // Mover el dron durante dt segundos:
    //   * Modo Espera: llamar a mantenerEspera(dt) y terminar.
    //   * Modo Manual: avanzar en su rumbo getVelocidad() * dt.
    //   * Modo Ruta con destino: volar en línea recta (rumbo exacto, no solo 8 direcciones) hacia
    //     el destino sin sobrepasarlo.
    //   * Cambiar la altura con acercarAltura(...), a config_.velocidadAscenso * dt.
}

// Avion.cpp - PLANTILLA PARA EL ALUMNO
//
// Las reglas del avión están en include/Avion.h. Completa los cuatro métodos.
// Herramientas de Aeronave que puedes usar aquí (ver include/Aeronave.h):
//   getModoControl(), tieneDestino(), getDestinoActual(), gradosHacia(punto), getRumboGrados(),
//   getVelocidad(), getAlturaObjetivo(), getRumboOrdenadoGrados(), setRumboOrdenadoGrados(),
//   setModoControl(), girarHaciaGrados(objetivo, maxGrados), avanzar(rumbo, distancia),
//   acercarAltura(objetivo, maxDelta), estaActiva(), y la configuración en config_.
#include <stdexcept>

#include "Avion.h"

bool Avion::puedeMoverse(Direccion direccion) const {
    (void)direccion;
    // TODO: IMPLEMENTAR POR EL ALUMNO
    // Un avión no puede invertir su rumbo de golpe: solo puede tomar direcciones a <= 90° de
    // su rumbo actual. (Pista: diferenciaAngular(...) de Direccion.h). Solo si estaActiva().
    return false;
}

void Avion::mover(Direccion direccion) {
    (void)direccion;
    // TODO: IMPLEMENTAR POR EL ALUMNO
    // 1. Si !puedeMoverse(direccion): lanzar std::invalid_argument.
    // 2. Pasar a ModoControl::Manual y guardar el rumbo pedido con setRumboOrdenadoGrados().
}

double Avion::calcularConsumo(double dt) const {
    (void)dt;
    // TODO: IMPLEMENTAR POR EL ALUMNO
    // Combustible gastado durante dt segundos. Un avión siempre está en vuelo:
    // consumo base + consumo proporcional a la velocidad (ver config_ y getVelocidad()).
    return 0.0;
}

void Avion::actualizar(double dt) {
    (void)dt;
    // TODO: IMPLEMENTAR POR EL ALUMNO
    // Mover el avión durante dt segundos:
    //   1. Decidir el rumbo deseado: en modo Manual, getRumboOrdenadoGrados(); en modo Ruta con
    //      destino, gradosHacia(getDestinoActual()); si no, mantener el rumbo actual.
    //   2. Girar hacia ese rumbo, como máximo config_.velocidadGiro * dt grados.
    //   3. Avanzar SIEMPRE en la dirección de su rumbo: getVelocidad() * dt casillas.
    //   4. Cambiar la altura de forma progresiva hacia getAlturaObjetivo().
}

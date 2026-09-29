// Ejemplo 4 - Colisiones. Tres situaciones a la vez:
//   1. AV-01 y DR-01 vuelan de frente a la misma altura        → COLISIÓN (llama 3D, estado Colisionada).
//   2. HE-01 y DR-02 se cruzan a la misma altura                → COLISIÓN.
//   3. AV-02 y AV-03 se cruzan con 20 unidades de diferencia    → se cruzan sin problema.
// Sin ATC, nadie evita nada: ¡este es el problema que resolverán los alumnos!
#include "comun.h"

int main() {
    Simulador simulador;
    Tablero tablero(60, 60);

    // 1. Choque frontal.
    auto& av1 = tablero.crear<Avion>("AV-01", Posicion{15, 55, 15}, Direccion::S);
    av1.setVelocidad(12.0);
    av1.agregarPuntoRuta({15, 5, 15});
    auto& dr1 = tablero.crear<Dron>("DR-01", Posicion{15, 5, 15});
    dr1.setVelocidad(6.0);
    dr1.agregarPuntoRuta({15, 55, 15});

    // 2. Cruce perpendicular a la misma altura.
    auto& he1 = tablero.crear<Helicoptero>("HE-01", Posicion{35, 30, 10});
    he1.agregarPuntoRuta({58, 30, 10});
    auto& dr2 = tablero.crear<Dron>("DR-02", Posicion{50, 10, 10});
    dr2.setVelocidad(6.0);
    dr2.agregarPuntoRuta({50, 50, 10});

    // 3. Cruce seguro por diferencia de altura.
    auto& av2 = tablero.crear<Avion>("AV-02", Posicion{30, 55, 40}, Direccion::S);
    av2.agregarPuntoRuta({30, 5, 40});
    auto& av3 = tablero.crear<Avion>("AV-03", Posicion{5, 30, 70}, Direccion::E);
    av3.agregarPuntoRuta({28, 30, 70});
    av3.agregarPuntoRuta({58, 30, 70});

    ejecutar(simulador, tablero, 20.0, [] {}, 0.05, 5.0);
    simulador.fin();
    return 0;
}

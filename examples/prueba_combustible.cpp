// Ejemplo 7 - Combustible. Cada tipo consume a su ritmo y, al agotarse, pasa a SinCombustible
// (queda inmóvil, en gris, y sigue siendo un obstáculo).
//   AV-01: siempre en vuelo, consumo alto.      HE-01: en vuelo estacionario, consumo elevado.
//   DR-01: consumo moderado, poco depósito.      HE-02: vuela despacio (gasta menos que HE-01).
// Orden en que se agotan: AV-01, DR-01, HE-01 (estacionario) y por último HE-02.
#include "comun.h"

int main() {
    Simulador simulador;
    Tablero tablero(60, 60);

    auto& av = tablero.crear<Avion>("AV-01", Posicion{5, 10, 30}, Direccion::E);
    av.setCombustible(15.0);
    av.agregarPuntoRuta({55, 10, 30});
    av.agregarPuntoRuta({55, 55, 30});

    auto& he1 = tablero.crear<Helicoptero>("HE-01", Posicion{10, 30, 10});
    he1.setCombustible(60.0);
    he1.setVelocidad(0.0);  // estacionario

    auto& he2 = tablero.crear<Helicoptero>("HE-02", Posicion{10, 40, 10});
    he2.setCombustible(60.0);
    he2.setVelocidad(1.0);
    he2.agregarPuntoRuta({55, 40, 10});

    auto& dr = tablero.crear<Dron>("DR-01", Posicion{10, 50, 5});
    dr.setCombustible(12.0);
    dr.setVelocidad(3.0);
    dr.agregarPuntoRuta({55, 50, 5});

    ejecutar(simulador, tablero, 50.0, [] {}, 0.05, 5.0);
    simulador.fin();
    return 0;
}

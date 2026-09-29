// Ejemplo 5 - Varias aeronaves de los tres tipos, con rutas, alturas, velocidades y combustible distintos.
// Cada aeronave tiene su color; puedes seleccionar cualquiera con un clic para ver su información.
#include "comun.h"

int main() {
    Simulador simulador;
    Tablero tablero(60, 60);

    // Aviones (altura 10..100)
    auto& av1 = tablero.crear<Avion>("AV-01", Posicion{5, 8, 25}, Direccion::E);
    av1.agregarPuntoRuta({55, 8, 25});
    av1.agregarPuntoRuta({55, 55, 35});
    av1.agregarPuntoRuta({8, 40, 30});

    auto& av2 = tablero.crear<Avion>("AV-02", Posicion{55, 50, 70}, Direccion::W);
    av2.setVelocidad(16.0);
    av2.agregarPuntoRuta({8, 50, 70});
    av2.agregarPuntoRuta({8, 10, 90});
    av2.agregarPuntoRuta({40, 10, 60});

    auto& av3 = tablero.crear<Avion>("AV-03", Posicion{30, 5, 50}, Direccion::N);
    av3.setVelocidad(8.0);
    av3.setCombustible(400.0);
    av3.agregarPuntoRuta({30, 55, 55});
    av3.agregarPuntoRuta({50, 30, 45});

    // Helicópteros (altura 0..40)
    auto& he1 = tablero.crear<Helicoptero>("HE-01", Posicion{10, 15, 8});
    he1.agregarPuntoRuta({25, 30, 12});
    he1.agregarPuntoRuta({10, 45, 20});

    auto& he2 = tablero.crear<Helicoptero>("HE-02", Posicion{50, 20, 36});
    he2.setVelocidad(3.0);
    he2.agregarPuntoRuta({50, 45, 36});

    // Drones (altura 0..20)
    auto& dr1 = tablero.crear<Dron>("DR-01", Posicion{20, 20, 3});
    dr1.setVelocidad(6.0);
    dr1.agregarPuntoRuta({45, 45, 16});
    dr1.agregarPuntoRuta({20, 48, 5});

    auto& dr2 = tablero.crear<Dron>("DR-02", Posicion{42, 12, 19});
    dr2.setCombustible(120.0);
    dr2.agregarPuntoRuta({12, 26, 19});

    auto& dr3 = tablero.crear<Dron>("DR-03", Posicion{25, 55, 1});
    dr3.esperarEn({28, 50, 2});

    ejecutar(simulador, tablero, 60.0, [] {}, 0.05, 15.0);
    simulador.fin();
    return 0;
}

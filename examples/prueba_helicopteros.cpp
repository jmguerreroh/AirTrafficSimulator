// Ejemplo 2 - Helicópteros: vuelo estacionario, movimiento solo en cardinales y consumo.
//
// HE-01 recorre una ruta (fíjate en que va "en escalera": nunca en diagonal).
// HE-02 permanece en vuelo estacionario: no cambia de posición pero SU COMBUSTIBLE BAJA (más deprisa
// que el de un helicóptero que vuela despacio).
#include "comun.h"

int main() {
    Simulador simulador;
    Tablero tablero(40, 40);

    auto& he1 = tablero.crear<Helicoptero>("HE-01", Posicion{5, 5, 5});
    he1.agregarPuntoRuta({30, 20, 20});  // en diagonal, pero solo puede moverse en N/E/S/W
    he1.agregarPuntoRuta({8, 32, 10});

    auto& he2 = tablero.crear<Helicoptero>("HE-02", Posicion{30, 8, 15});
    he2.setVelocidad(0.0);  // vuelo estacionario

    ejecutar(simulador, tablero, 40.0, [] {}, 0.05, 10.0);
    simulador.fin();
    return 0;
}

// Ejemplo 1 - Dos aviones siguiendo rutas sencillas a distinta altura.
//
// Observa: los aviones giran con suavidad (no pueden ir de lado ni pararse), suben y bajan de forma
// progresiva, la casilla recorrida se pinta con el color del avión y el destino actual lleva su id.
#include "comun.h"

int main() {
    Simulador simulador;
    Tablero tablero(60, 60);

    auto& av1 = tablero.crear<Avion>("AV-01", Posicion{5, 10, 30}, Direccion::E);
    av1.agregarPuntoRuta({50, 10, 30});
    av1.agregarPuntoRuta({50, 50, 50});
    av1.agregarPuntoRuta({10, 50, 40});

    auto& av2 = tablero.crear<Avion>("AV-02", Posicion{55, 30, 70}, Direccion::W);
    av2.setVelocidad(14.0);
    av2.agregarPuntoRuta({10, 30, 70});
    av2.agregarPuntoRuta({15, 10, 55});

    ejecutar(simulador, tablero, 40.0, [] {}, 0.05, 10.0);
    simulador.fin();
    return 0;
}

// Ejemplo 3 - Drones: movimiento 3D libre, altura rápida y modo espera.
//
// DR-01 vuela en línea recta en diagonal cambiando de altura deprisa.
// DR-02 recibe un punto de espera y "vigila" la zona (gastando combustible) hasta que se le
// devuelve el control de su ruta.
#include "comun.h"

int main() {
    Simulador simulador;
    Tablero tablero(50, 50);

    auto& dr1 = tablero.crear<Dron>("DR-01", Posicion{3, 3, 1});
    dr1.setVelocidad(7.0);
    dr1.agregarPuntoRuta({40, 40, 18});
    dr1.agregarPuntoRuta({5, 42, 4});

    auto& dr2 = tablero.crear<Dron>("DR-02", Posicion{40, 5, 10});
    dr2.agregarPuntoRuta({8, 8, 12});
    dr2.esperarEn({25, 20, 14});  // vigilancia alrededor de (25, 20)

    bool liberado = false;
    ejecutar(simulador, tablero, 35.0, [&] {
        if (!liberado && tablero.getTiempo() > 15.0) {  // a los 15 s se le devuelve la ruta
            dr2.cancelarEspera();
            liberado = true;
        }
    }, 0.05, 10.0);
    simulador.fin();
    return 0;
}

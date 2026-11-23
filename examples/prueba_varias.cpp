// Ejemplo 5 - Varias aeronaves de los tres tipos, con rutas, alturas, velocidades y combustible distintos.
// Cada aeronave tiene su color; puedes seleccionar cualquiera con un clic para ver su información.
// La escena usa TU ATC (las aeronaves están en examples/escenas.h).
#include "comun.h"
#include "escenas.h"

#include <ATC.h>

int main() {
    Simulador simulador;
    Tablero tablero(60, 60);
    escenaVarias(tablero);

    ATC atc;
    const double dt = 0.05;
    ejecutar(simulador, tablero, 60.0, [&] { atc.actualizar(tablero.getAeronaves(), dt); }, dt, 15.0);
    simulador.fin();
    return 0;
}

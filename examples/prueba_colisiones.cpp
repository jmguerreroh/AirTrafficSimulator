// Ejemplo 4 - Colisiones. Tres situaciones a la vez:
//   1. AV-01 y DR-01 vuelan de frente a la misma altura        → COLISIÓN (llama 3D, estado Colisionada).
//   2. HE-01 y DR-02 se cruzan a la misma altura                → COLISIÓN.
//   3. AV-02 y AV-03 se cruzan con 20 unidades de diferencia    → se cruzan sin problema.
// La escena usa TU ATC: mientras ATC::actualizar no haga nada, nadie evita nada y se producen los dos choques.
// Cuando tu ATC funcione, no debe haber ninguna colisión.
#include "comun.h"
#include "escenas.h"

#include <ATC.h>

int main() {
    Simulador simulador;
    Tablero tablero(60, 60);
    escenaColisiones(tablero);  // las tres situaciones de arriba (examples/escenas.h)

    ATC atc;
    const double dt = 0.05;
    ejecutar(simulador, tablero, 20.0, [&] { atc.actualizar(tablero.getAeronaves(), dt); }, dt, 5.0);
    std::cout << "Colisiones: " << tablero.getColisiones().size() << "\n";
    simulador.fin();
    return 0;
}

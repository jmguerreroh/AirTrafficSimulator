// main.cpp - TU PROGRAMA. Crea el mundo, el ATC y el bucle de simulación (ver docs/PRACTICA_ATC.md).
//
//   ATC                  → decide (src/ATC.cpp)
//   Tablero::avanzar(dt) → el simulador mueve el mundo, consume combustible y detecta colisiones
//   Simulador            → muestra el resultado (no bloquea)

#include <Simulador.h>

#include <Avion.h>
#include <ATC.h>
#include <Dron.h>
#include <Helicoptero.h>

#include <chrono>
#include <iostream>
#include <thread>

int main() {
    Simulador simulador;
    Tablero tablero(60, 60);

    // Crear aeronaves y rutas -------------------------------------------------------------
    auto& av1 = tablero.crear<Avion>("AV-01", Posicion{5, 30, 30}, Direccion::E);
    av1.agregarPuntoRuta({55, 30, 30});
    auto& av2 = tablero.crear<Avion>("AV-02", Posicion{55, 30, 30}, Direccion::W);  // ¡misma altura y de frente!
    av2.agregarPuntoRuta({5, 30, 30});
    auto& he1 = tablero.crear<Helicoptero>("HE-01", Posicion{30, 5, 10});
    he1.agregarPuntoRuta({30, 55, 10});
    auto& dr1 = tablero.crear<Dron>("DR-01", Posicion{10, 50, 5});
    dr1.agregarPuntoRuta({50, 10, 15});

    // Crear el ATC ---------------------------------------------------------------------------
    ATC atc;

    // Bucle principal ------------------------------------------------------------------------
    const double dt = 0.05;  // segundos de simulación por iteración
    simulador.setTablero(tablero);
    while (simulador.abierto() && tablero.getTiempo() < 60.0) {
        atc.actualizar(tablero.getAeronaves(), dt);  // 1. el ATC decide
        tablero.avanzar(dt);                         // 2. el mundo avanza
        simulador.setTablero(tablero);               // 3. se muestra (no bloquea)
        std::this_thread::sleep_for(std::chrono::duration<double>(dt));
    }

    std::cout << "Colisiones: " << tablero.getColisiones().size() << "\n";
    std::cout << "Fuera del tablero: " << tablero.getSalidas().size() << "\n";
    simulador.esperarCierre();
    simulador.fin();
    return 0;
}

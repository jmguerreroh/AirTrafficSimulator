// comun.h - Utilidades compartidas por las pruebas (el bucle habitual y un resumen por consola).
#pragma once

#include <Avion.h>
#include <Dron.h>
#include <Helicoptero.h>
#include <Simulador.h>

#include <chrono>
#include <cstdio>
#include <iostream>
#include <thread>

// Tabla con el estado de todas las aeronaves.
inline void mostrarResumen(const Tablero& t) {
    std::printf("\n t=%6.1fs | %-6s %-12s %-15s %7s %7s %6s\n", t.getTiempo(), "ID", "TIPO", "ESTADO", "ALTURA", "COMB.%", "RUTA");
    for (const auto& a : t.getAeronaves()) {
        std::printf("          | %-6s %-12s %-15s %7.1f %6.0f%% %3zu/%-3zu\n", a->getId().c_str(), aTexto(a->getTipo()),
                    aTexto(a->getEstado()), a->getAltura(), a->getPorcentajeCombustible(), a->getIndiceRuta(), a->getRuta().size());
    }
    std::printf("          | colisiones registradas: %zu | fuera del tablero: %zu\n", t.getColisiones().size(),
                t.getSalidas().size());
}

// Bucle estándar de las pruebas: en cada iteración se llama a 'porPaso' (aquí iría el ATC), se avanza
// la simulación 'dt' segundos y se muestra el resultado. setTablero() NO bloquea: la pausa la ponemos
// nosotros para que la animación transcurra a velocidad de "tiempo real".
template <class F>
void ejecutar(Simulador& sim, Tablero& tablero, double segundos, F&& porPaso, double dt = 0.05, double cadaSegundos = 0.0) {
    double proximoResumen = cadaSegundos;
    while (sim.abierto() && tablero.getTiempo() < segundos) {
        porPaso();
        tablero.avanzar(dt);
        sim.setTablero(tablero);
        if (cadaSegundos > 0.0 && tablero.getTiempo() >= proximoResumen && tablero.getTiempo() < segundos - 0.2) {
            mostrarResumen(tablero);
            proximoResumen += cadaSegundos;
        }
#ifndef ATC_SIN_ESPERA  // (solo se define en pruebas automáticas sin ventana)
        std::this_thread::sleep_for(std::chrono::duration<double>(dt));
#endif
    }
    mostrarResumen(tablero);
    if (sim.abierto()) {
        std::cout << "\nFin de la prueba. Cierra la ventana para terminar.\n";
        sim.esperarCierre();
    }
}

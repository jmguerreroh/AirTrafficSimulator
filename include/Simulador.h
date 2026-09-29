// Simulador.h - Ventana 3D del simulador de tráfico aéreo.
//
// El alumno sólo necesita esto:
//
//     Simulador simulador;
//     Tablero tablero(50, 50);
//     tablero.crear<Avion>("AV-01", Posicion{5, 5, 30}, Direccion::E).agregarPuntoRuta({40, 5, 30});
//     while (simulador.abierto()) {
//         atc.actualizar(tablero.getAeronaves(), dt);
//         tablero.avanzar(dt);
//         simulador.setTablero(tablero);   // NO bloquea
//     }
//     simulador.fin();
//
// La ventana se abre en la primera llamada a setTablero() y se dibuja en su propio hilo.
#pragma once

#include <memory>

#include "Tablero.h"

class Simulador {
public:
    Simulador();
    ~Simulador();  // equivale a fin()

    Simulador(const Simulador&) = delete;
    Simulador& operator=(const Simulador&) = delete;
    Simulador(Simulador&&) = delete;
    Simulador& operator=(Simulador&&) = delete;

    // Muestra el estado actual del tablero. Hace una COPIA (el tablero es tuyo y puedes seguir
    // modificándolo) y vuelve enseguida. La primera llamada abre la ventana.
    // Después de fin() o de cerrar la ventana no hace nada.
    void setTablero(const Tablero& tablero);
    // Igual, comprobando que las dimensiones coinciden (std::invalid_argument si no).
    void setTablero(const Tablero& tablero, int tamX, int tamY);

    // Cierra la ventana y libera todo. Puede llamarse varias veces.
    void fin();
    // false cuando la ventana se cerró (por el usuario o por fin()); true antes de abrirla.
    bool abierto() const;
    // Bloquea hasta que el usuario cierre la ventana (vuelve al instante si no hay ninguna).
    void esperarCierre();

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

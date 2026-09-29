#include "Simulador.h"

#include <atomic>
#include <chrono>
#include <mutex>
#include <stdexcept>
#include <thread>

#include "RenderLoop.h"
#include "SharedScene.h"

// Estados del simulador (ver docs/ARQUITECTURA.md):
//
//   Idle --setTablero--> Running --fin()/ventana cerrada--> Closed
//     \------------------fin()---------------------------------^
//
// Closed es terminal: setTablero() sigue validando las dimensiones pero no hace nada más.
struct Simulador::Impl {
    enum class State { Idle, Running, Closed };

    SharedScene shared;
    RenderLoop loop{shared};
    std::mutex mutex;  // serializa publish / fin / reap; el hilo de render NUNCA lo toma
    std::atomic<State> state{State::Idle};

    void publish(const Tablero& tablero) {
        std::lock_guard<std::mutex> lock(mutex);
        if (state.load() == State::Closed) return;
        if (state.load() == State::Running && !loop.running()) {  // el usuario cerró la ventana
            reap();
            return;
        }
        // Copia profunda en el hilo del alumno (el render nunca toca el Tablero original).
        auto snapshot = std::make_shared<const Tablero>(tablero.clonar());
        shared.publish(std::move(snapshot));
        if (state.load() == State::Idle) {  // primera llamada: abrir la ventana
            try {
                loop.start();
            } catch (...) {
                state.store(State::Closed);
                throw;
            }
            state.store(State::Running);
        }
    }

    void fin() {
        std::lock_guard<std::mutex> lock(mutex);
        if (state.load() == State::Running) {
            loop.requestStop();
            reap();
        }
        state.store(State::Closed);
    }

    bool isOpen() const {
        switch (state.load()) {
            case State::Idle: return true;
            case State::Running: return loop.running();
            case State::Closed: return false;
        }
        return false;
    }

    void waitForClose() {
        if (state.load() != State::Running) return;
        while (loop.running()) std::this_thread::sleep_for(std::chrono::milliseconds(20));
        std::lock_guard<std::mutex> lock(mutex);
        if (state.load() == State::Running) reap();
    }

private:
    void reap() {  // precondición: 'mutex' tomado
        loop.join();
        state.store(State::Closed);
    }
};

Simulador::Simulador() : impl_(std::make_unique<Impl>()) {}

Simulador::~Simulador() {
    try {
        impl_->fin();
    } catch (...) {
        // Un destructor no debe lanzar excepciones.
    }
}

void Simulador::setTablero(const Tablero& tablero) { impl_->publish(tablero); }

void Simulador::setTablero(const Tablero& tablero, int tamX, int tamY) {
    if (tamX != tablero.getTamX() || tamY != tablero.getTamY())
        throw std::invalid_argument("Simulador::setTablero: las dimensiones indicadas (" + std::to_string(tamX) + "x" +
                                    std::to_string(tamY) + ") no coinciden con las del tablero (" +
                                    std::to_string(tablero.getTamX()) + "x" + std::to_string(tablero.getTamY()) + ")");
    impl_->publish(tablero);
}

void Simulador::fin() { impl_->fin(); }

bool Simulador::abierto() const { return impl_->isOpen(); }

void Simulador::esperarCierre() { impl_->waitForClose(); }

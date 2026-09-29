// SharedScene.h - Único punto de intercambio entre el hilo del alumno y el hilo de render.
//
// Arquitectura "snapshot": el alumno publica una COPIA PROFUNDA e inmutable del Tablero
// (std::shared_ptr<const Tablero>); el render recoge el puntero de la última copia. Bajo el mutex solo
// se intercambian punteros (nanosegundos); nadie modifica jamás una copia ya publicada, así que el
// render puede leerla sin bloqueo mientras el ATC sigue modificando su Tablero original.
// Si el alumno publica más rápido que el render, se dibuja siempre la última (estado coherente).
#pragma once

#include <cstdint>
#include <memory>
#include <mutex>
#include <utility>

#include "Tablero.h"

class SharedScene {
public:
    void publish(std::shared_ptr<const Tablero> snapshot) {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            std::swap(snapshot_, snapshot);
            ++version_;
        }
        // 'snapshot' contiene ahora la copia antigua y se libera fuera del mutex.
    }

    bool fetchIfNewer(std::uint64_t& seenVersion, std::shared_ptr<const Tablero>& out) const {
        std::lock_guard<std::mutex> lock(mutex_);
        if (version_ == seenVersion) return false;
        out = snapshot_;
        seenVersion = version_;
        return true;
    }

private:
    mutable std::mutex mutex_;
    std::shared_ptr<const Tablero> snapshot_;
    std::uint64_t version_ = 0;
};

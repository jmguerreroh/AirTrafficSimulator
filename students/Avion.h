// Avion.h - Aeronave de ala fija.
//
// REGLAS DEL AVIÓN (lo que debe implementar Avion.cpp):
//   * Siempre avanza en la dirección de su rumbo: NO puede moverse lateralmente ni quedarse quieto.
//   * Debe mantener al menos velocidadMinima.
//   * Gira a izquierda/derecha como mucho velocidadGiro grados por segundo.
//   * Sube y baja de forma progresiva (velocidadAscenso) dentro de sus límites de altura.
//   * No puede invertir su rumbo de golpe: puedeMoverse(d) solo es cierto si d está a ≤ 90° del rumbo actual.
//   * Consume combustible siempre (base + proporcional a la velocidad).
#pragma once

#include <stdexcept>

#include "Aeronave.h"

class Avion : public Aeronave {
public:
    Avion(const std::string& id, const Posicion& posicion, Direccion rumbo,
          const ConfiguracionAvion& config = ConfiguracionAvion())
        : Aeronave(id, posicion, rumbo, config), config_(config) {
        if (config.velocidadMinima <= 0.0) throw std::invalid_argument("Avion: la velocidad minima debe ser > 0");
        if (config.velocidadGiro <= 0.0) throw std::invalid_argument("Avion: la velocidad de giro debe ser > 0");
    }

    TipoAeronave getTipo() const override { return TipoAeronave::Avion; }
    const ConfiguracionAvion& getConfiguracionAvion() const { return config_; }
    std::unique_ptr<Aeronave> clonar() const override { return std::make_unique<Avion>(*this); }

    void actualizar(double dt) override;                  // ← alumno
    bool puedeMoverse(Direccion direccion) const override;  // ← alumno
    void mover(Direccion direccion) override;             // ← alumno
    double calcularConsumo(double dt) const override;     // ← alumno

private:
    ConfiguracionAvion config_;
};

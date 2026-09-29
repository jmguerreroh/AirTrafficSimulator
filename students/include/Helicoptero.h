// Helicoptero.h - Aeronave de ala rotatoria.
//
// REGLAS DEL HELICÓPTERO (lo que debe implementar Helicoptero.cpp):
//   * Se desplaza en las 4 direcciones cardinales (N, E, S, W): avanzar, retroceder y lateral.
//     NO se mueve en diagonal (para ir en diagonal tiene que hacer una escalera).
//   * Puede cambiar de altura sin desplazarse horizontalmente.
//   * Puede estar en vuelo estacionario (velocidad 0): no cambia de posición, PERO gasta combustible
//     (consumoEstacionario, más alto que volar despacio).
//   * Puede cambiar de rumbo al instante.
#pragma once

#include "Aeronave.h"

class Helicoptero : public Aeronave {
public:
    Helicoptero(const std::string& id, const Posicion& posicion, Direccion rumbo = Direccion::N,
                const ConfiguracionHelicoptero& config = ConfiguracionHelicoptero())
        : Aeronave(id, posicion, rumbo, config), config_(config) {}

    TipoAeronave getTipo() const override { return TipoAeronave::Helicoptero; }
    const ConfiguracionHelicoptero& getConfiguracionHelicoptero() const { return config_; }
    std::unique_ptr<Aeronave> clonar() const override { return std::make_unique<Helicoptero>(*this); }

    void actualizar(double dt) override;                  // ← alumno
    bool puedeMoverse(Direccion direccion) const override;  // ← alumno
    void mover(Direccion direccion) override;             // ← alumno
    double calcularConsumo(double dt) const override;     // ← alumno

private:
    ConfiguracionHelicoptero config_;
};

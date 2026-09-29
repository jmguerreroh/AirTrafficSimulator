// Dron.h - Aeronave pequeña y ágil.
//
// REGLAS DEL DRON (lo que debe implementar Dron.cpp):
//   * Se desplaza en cualquier dirección, también en diagonal (8 direcciones y rumbos intermedios).
//   * Puede subir y bajar muy deprisa (velocidadAscenso alta) y quedarse estacionario.
//   * Tiene poca autonomía (combustibleMaximo pequeño).
//   * MODO ESPERA: esperarEn(centro) lo hace permanecer en una zona de radio 'radioEspera'
//     alrededor de un punto, gastando combustible (vigilancia, patrulla, reconocimiento...).
//     La infraestructura ya está hecha; DECIDIR cuándo usarla es cosa del ATC del alumno.
#pragma once

#include <algorithm>
#include <cmath>

#include "Aeronave.h"

class Dron : public Aeronave {
public:
    Dron(const std::string& id, const Posicion& posicion, Direccion rumbo = Direccion::N,
         const ConfiguracionDron& config = ConfiguracionDron())
        : Aeronave(id, posicion, rumbo, config), config_(config) {}

    TipoAeronave getTipo() const override { return TipoAeronave::Dron; }
    const ConfiguracionDron& getConfiguracionDron() const { return config_; }
    std::unique_ptr<Aeronave> clonar() const override { return std::make_unique<Dron>(*this); }

    // ---- Modo espera (infraestructura ya implementada) ------------------------------
    void esperarEn(const Posicion& centro) {  // pasa a ModoControl::Espera
        validarPunto(centro);
        centro_ = centro;
        setAlturaObjetivo(centro.z);
        setModoControl(ModoControl::Espera);
    }
    void cancelarEspera() { seguirRuta(); }
    bool enEspera() const { return getModoControl() == ModoControl::Espera; }
    const Posicion& getCentroEspera() const { return centro_; }

    void actualizar(double dt) override;                  // ← alumno
    bool puedeMoverse(Direccion direccion) const override;  // ← alumno
    void mover(Direccion direccion) override;             // ← alumno
    double calcularConsumo(double dt) const override;     // ← alumno

protected:
    // Un paso de "orbitar alrededor de centro" (ya implementado; llámalo desde actualizar()
    // cuando getModoControl() == ModoControl::Espera).
    void mantenerEspera(double dt) {
        const double radio = config_.radioEspera;
        const double dx = getX() - centro_.x, dy = getY() - centro_.y;
        double objetivoGrados;
        if (std::hypot(dx, dy) > 1.5 * radio) {
            objetivoGrados = gradosHacia(centro_);  // aún lejos: acercarse a la zona
        } else {
            // Dentro de la zona: apuntar a un punto algo más adelante sobre la circunferencia.
            const double angulo = std::atan2(dx, dy) + 0.6;
            const Posicion adelante{static_cast<int>(std::lround(centro_.x + radio * std::sin(angulo))),
                                    static_cast<int>(std::lround(centro_.y + radio * std::cos(angulo))), centro_.z};
            objetivoGrados = gradosHacia(adelante);
        }
        fijarRumboGrados(objetivoGrados);
        avanzar(objetivoGrados, getVelocidad() * dt);
        acercarAltura(centro_.z, config_.velocidadAscenso * dt);
    }

private:
    ConfiguracionDron config_;
    Posicion centro_;
};

// Tablero.h - El mundo: espacio aéreo tamX × tamY × altura, con sus aeronaves.
//
// El Tablero es el "estado lógico" de la simulación. Vive en el hilo del alumno; el simulador
// gráfico sólo recibe COPIAS suyas (Simulador::setTablero), nunca lo toca directamente.
//
// Es POLIMÓRFICO: guarda std::unique_ptr<Aeronave> y sólo usa la interfaz de Aeronave; no
// conoce Avion, Helicoptero ni Dron.
#pragma once

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "Aeronave.h"
#include "Color.h"
#include "Configuracion.h"

// Casilla del suelo: recuerda quién pasó por última vez (historial de trayectorias).
struct CeldaHistorial {
    bool visitada = false;
    Color color;             // color de la última aeronave que estuvo allí
    std::string idUltimo;    // id de esa aeronave
};

// Una aeronave que se ha salido del tablero: se estrella en el borde (estado FueraDeLimites).
struct Salida {
    std::string id;
    double x = 0, y = 0, z = 0;  // punto del borde donde se estrelló
    double tiempo = 0;           // instante de simulación en el que ocurrió
};

struct Colision {
    std::string idA;
    std::string idB;
    double x = 0, y = 0, z = 0;  // punto medio
    double tiempo = 0;           // instante de simulación en el que ocurrió
};

class Tablero {
public:
    static constexpr int kTamMaximo = 200;

    // Lanza std::invalid_argument si tamX/tamY no están en [1, kTamMaximo].
    Tablero(int tamX, int tamY, const ConfiguracionSimulacion& config = ConfiguracionSimulacion());

    Tablero(Tablero&&) noexcept = default;
    Tablero& operator=(Tablero&&) noexcept = default;
    Tablero(const Tablero&) = delete;
    Tablero& operator=(const Tablero&) = delete;
    Tablero clonar() const;  // copia profunda (polimórfica)

    int getTamX() const { return tamX_; }
    int getTamY() const { return tamY_; }
    const ConfiguracionSimulacion& getConfiguracion() const { return config_; }
    double getTiempo() const { return tiempo_; }  // segundos de simulación transcurridos

    // ---- Aeronaves -------------------------------------------------------------------
    // Toma posesión de la aeronave. Valida: no nula, id único (si está vacío se asigna
    // "AV-01"...), posición y ruta dentro del tablero, color único (si no tiene, se le asigna uno).
    // Lanza std::invalid_argument si algo no es válido.
    Aeronave& agregar(std::unique_ptr<Aeronave> aeronave);

    // Atajo:  auto& a = tablero.crear<Avion>("AV-01", Posicion{5, 5, 30}, Direccion::E);
    template <class T, class... Args>
    T& crear(Args&&... args) {
        return static_cast<T&>(agregar(std::make_unique<T>(std::forward<Args>(args)...)));
    }

    std::vector<std::unique_ptr<Aeronave>>& getAeronaves() { return aeronaves_; }
    const std::vector<std::unique_ptr<Aeronave>>& getAeronaves() const { return aeronaves_; }
    Aeronave* buscar(const std::string& id);
    const Aeronave* buscar(const std::string& id) const;

    // ---- Simulación ------------------------------------------------------------------
    // Avanza el mundo 'dt' segundos (dt > 0). Internamente se divide en pasos de como mucho
    // config.pasoMaximo para que ninguna aeronave "atraviese" a otra sin detectarse.
    // No depende de los FPS de la ventana: sólo de dt.
    void avanzar(double dt);

    bool posicionValida(const Posicion& p) const;
    const std::vector<Colision>& getColisiones() const { return colisiones_; }  // todas, en orden
    const std::vector<Salida>& getSalidas() const { return salidas_; }           // aeronaves que se salieron
    const CeldaHistorial& getCelda(int x, int y) const;                        // std::out_of_range si fuera

private:
    void paso(double h);
    void registrarColisiones();

    int tamX_;
    int tamY_;
    ConfiguracionSimulacion config_;
    double tiempo_ = 0.0;
    std::vector<std::unique_ptr<Aeronave>> aeronaves_;
    std::vector<CeldaHistorial> celdas_;
    std::vector<Colision> colisiones_;
    std::vector<Salida> salidas_;
};

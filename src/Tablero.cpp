#include "Tablero.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <stdexcept>

#include "simulation/CollisionSystem.h"

Tablero::Tablero(int tamX, int tamY, const ConfiguracionSimulacion& config)
    : tamX_(tamX), tamY_(tamY), config_(config) {
    if (tamX < 1 || tamY < 1 || tamX > kTamMaximo || tamY > kTamMaximo)
        throw std::invalid_argument("Tablero: el tamano debe estar entre 1 y " + std::to_string(kTamMaximo) +
                                    " (recibido " + std::to_string(tamX) + "x" + std::to_string(tamY) + ")");
    if (config.pasoMaximo <= 0.0 || config.umbralColisionAltura < 0.0 || config.duracionExplosion < 0.0 ||
        config.alturaMaximaTablero < 1)
        throw std::invalid_argument("Tablero: ConfiguracionSimulacion invalida");
    celdas_.resize(static_cast<std::size_t>(tamX) * static_cast<std::size_t>(tamY));
}

Tablero Tablero::clonar() const {
    Tablero copia(tamX_, tamY_, config_);
    copia.tiempo_ = tiempo_;
    copia.celdas_ = celdas_;
    copia.colisiones_ = colisiones_;
    copia.aeronaves_.reserve(aeronaves_.size());
    for (const auto& a : aeronaves_) copia.aeronaves_.push_back(a->clonar());
    return copia;
}

bool Tablero::posicionValida(const Posicion& p) const {
    return p.x >= 0 && p.y >= 0 && p.x < tamX_ && p.y < tamY_ && p.z >= 0 && p.z <= config_.alturaMaximaTablero;
}

const CeldaHistorial& Tablero::getCelda(int x, int y) const {
    if (x < 0 || y < 0 || x >= tamX_ || y >= tamY_) throw std::out_of_range("Tablero::getCelda fuera de rango");
    return celdas_[static_cast<std::size_t>(y) * tamX_ + x];
}

Aeronave* Tablero::buscar(const std::string& id) {
    for (auto& a : aeronaves_)
        if (a->getId() == id) return a.get();
    return nullptr;
}

const Aeronave* Tablero::buscar(const std::string& id) const {
    for (const auto& a : aeronaves_)
        if (a->getId() == id) return a.get();
    return nullptr;
}

Aeronave& Tablero::agregar(std::unique_ptr<Aeronave> aeronave) {
    if (!aeronave) throw std::invalid_argument("Tablero::agregar: aeronave nula");

    if (aeronave->getId().empty()) {  // id automático: AV-01, AV-02...
        int n = static_cast<int>(std::count_if(aeronaves_.begin(), aeronaves_.end(),
                                               [&](const auto& a) { return a->getTipo() == aeronave->getTipo(); })) + 1;
        std::string id;
        do {
            char buf[32];
            std::snprintf(buf, sizeof buf, "%s-%02d", prefijoId(aeronave->getTipo()), n++);
            id = buf;
        } while (buscar(id) != nullptr);
        aeronave->asignarId(id);
    } else if (buscar(aeronave->getId()) != nullptr) {
        throw std::invalid_argument("Ya existe una aeronave con el id " + aeronave->getId());
    }

    if (!posicionValida(aeronave->getPosicion()))
        throw std::invalid_argument("La aeronave " + aeronave->getId() + " empieza fuera del tablero: " +
                                    std::to_string(aeronave->getX()) + ", " + std::to_string(aeronave->getY()));
    if (aeronave->getAltura() > config_.alturaMaximaTablero)
        throw std::invalid_argument("La aeronave " + aeronave->getId() + " supera la altura maxima del tablero");
    for (const PuntoRuta& p : aeronave->getRuta()) {
        if (!posicionValida(p))
            throw std::invalid_argument("La ruta de " + aeronave->getId() + " sale del tablero en " +
                                        std::to_string(p.x) + ", " + std::to_string(p.y) + ", " + std::to_string(p.z));
    }

    auto colorEnUso = [&](const Color& c) {
        return std::any_of(aeronaves_.begin(), aeronaves_.end(), [&](const auto& a) { return a->getColor() == c; });
    };
    if (!aeronave->tieneColor()) {
        std::size_t i = 0;
        while (colorEnUso(colorPaleta(i))) ++i;
        aeronave->setColor(colorPaleta(i));
    } else if (colorEnUso(aeronave->getColor())) {
        throw std::invalid_argument("El color de " + aeronave->getId() + " ya lo usa otra aeronave");
    }

    aeronave->establecerLimitesTablero(tamX_, tamY_);
    aeronaves_.push_back(std::move(aeronave));
    return *aeronaves_.back();
}

void Tablero::avanzar(double dt) {
    if (!(dt > 0.0) || !std::isfinite(dt)) throw std::invalid_argument("Tablero::avanzar: dt debe ser > 0");
    const int subpasos = std::max(1, static_cast<int>(std::ceil(dt / config_.pasoMaximo)));
    const double h = dt / subpasos;
    for (int i = 0; i < subpasos; ++i) paso(h);
}

void Tablero::paso(double h) {
    for (auto& ptr : aeronaves_) {
        Aeronave& a = *ptr;
        if (!a.estaActiva()) continue;  // Llegada, SinCombustible y Colisionada no se mueven

        const double x0 = a.getX(), y0 = a.getY();
        a.actualizar(h);                // ← lógica de movimiento de cada tipo
        a.acotarAlTablero();

        const bool seMovio = std::hypot(a.getX() - x0, a.getY() - y0) > 1e-9;
        a.setEstado(seMovio ? EstadoAeronave::Volando : EstadoAeronave::Estacionaria);

        a.consumirCombustible(a.calcularConsumo(h));  // ← consumo de cada tipo
        a.comprobarLlegada();
    }

    // Historial: la casilla toma el color de la última aeronave que la pisó.
    for (auto& ptr : aeronaves_) {
        const Aeronave& a = *ptr;
        const Posicion p = a.getPosicion();
        if (p.x < 0 || p.y < 0 || p.x >= tamX_ || p.y >= tamY_) continue;
        CeldaHistorial& c = celdas_[static_cast<std::size_t>(p.y) * tamX_ + p.x];
        c.visitada = true;
        c.color = a.getColor();
        c.idUltimo = a.getId();
    }

    tiempo_ += h;
    registrarColisiones();
}

void Tablero::registrarColisiones() {
    for (const auto& [i, j] : detectarColisiones(aeronaves_, config_)) {
        Aeronave& a = *aeronaves_[i];
        Aeronave& b = *aeronaves_[j];
        Colision c;
        c.idA = a.getId();
        c.idB = b.getId();
        c.x = 0.5 * (a.getX() + b.getX());
        c.y = 0.5 * (a.getY() + b.getY());
        c.z = 0.5 * (a.getAltura() + b.getAltura());
        c.tiempo = tiempo_;
        colisiones_.push_back(c);
        a.setEstado(EstadoAeronave::Colisionada);
        b.setEstado(EstadoAeronave::Colisionada);
    }
}

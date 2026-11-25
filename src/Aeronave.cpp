#include "Aeronave.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <stdexcept>

namespace {

constexpr double kPi = 3.14159265358979323846;

void validarConfiguracion(const ConfiguracionAeronave& c) {
    auto falla = [](const char* msg) { throw std::invalid_argument(std::string("Configuracion invalida: ") + msg); };
    if (c.velocidadMinima < 0 || c.velocidadMaxima < c.velocidadMinima) falla("velocidades");
    if (c.velocidadInicial < c.velocidadMinima || c.velocidadInicial > c.velocidadMaxima)
        falla("velocidadInicial fuera de [velocidadMinima, velocidadMaxima]");
    if (c.alturaMinima < 0 || c.alturaMaxima < c.alturaMinima) falla("alturas");
    if (c.velocidadAscenso <= 0) falla("velocidadAscenso debe ser > 0");
    if (c.velocidadGiro < 0) falla("velocidadGiro");
    if (c.combustibleMaximo <= 0) falla("combustibleMaximo debe ser > 0");
    if (c.consumoBase < 0 || c.consumoPorVelocidad < 0 || c.consumoEstacionario < 0) falla("consumos");
    if (c.toleranciaLlegadaXY <= 0 || c.toleranciaLlegadaZ <= 0) falla("tolerancias de llegada");
}

}  // namespace

Aeronave::Aeronave(const std::string& id, const Posicion& p, Direccion rumbo, const ConfiguracionAeronave& cfg)
    : id_(id),
      cfg_(cfg),
      x_(p.x),
      y_(p.y),
      z_(p.z),
      velocidad_(cfg.velocidadInicial),
      combustible_(cfg.combustibleMaximo),
      rumboGrados_(gradosDe(rumbo)),
      rumboOrdenado_(gradosDe(rumbo)),
      alturaObjetivo_(p.z) {
    validarConfiguracion(cfg);
    if (id.size() > 12) throw std::invalid_argument("El id no puede tener mas de 12 caracteres");
    for (char c : id) {
        if (!(std::isalnum(static_cast<unsigned char>(c)) || c == '-' || c == '_'))
            throw std::invalid_argument("El id solo admite letras, numeros, '-' y '_': " + id);
    }
    if (p.x < 0 || p.y < 0) throw std::invalid_argument("Posicion inicial con coordenadas negativas");
    if (p.z < cfg.alturaMinima || p.z > cfg.alturaMaxima)
        throw std::invalid_argument("Altura inicial " + std::to_string(p.z) + " fuera de los limites [" +
                                    std::to_string(cfg.alturaMinima) + ", " + std::to_string(cfg.alturaMaxima) +
                                    "] de este tipo de aeronave");
}

Posicion Aeronave::getPosicion() const {
    return {static_cast<int>(std::lround(x_)), static_cast<int>(std::lround(y_)), static_cast<int>(std::lround(z_))};
}

// ------------------------------------------------------------------------------ ruta

void Aeronave::validarPunto(const Posicion& p) const {
    if (p.x < 0 || p.y < 0 || p.x >= limiteX_ || p.y >= limiteY_)
        throw std::invalid_argument("Punto fuera del tablero: (" + std::to_string(p.x) + ", " + std::to_string(p.y) + ")");
    if (p.z < cfg_.alturaMinima || p.z > cfg_.alturaMaxima)
        throw std::invalid_argument("Altura " + std::to_string(p.z) + " fuera de los limites [" +
                                    std::to_string(cfg_.alturaMinima) + ", " + std::to_string(cfg_.alturaMaxima) +
                                    "] de este tipo de aeronave");
}

const PuntoRuta& Aeronave::getDestinoActual() const {
    if (!tieneDestino()) throw std::logic_error("La aeronave " + id_ + " no tiene destino actual");
    return ruta_[indiceRuta_];
}

void Aeronave::agregarPuntoRuta(const PuntoRuta& p) {
    validarPunto(p);
    ruta_.push_back(p);
    if (estado_ == EstadoAeronave::Llegada) estado_ = EstadoAeronave::Volando;  // tiene trabajo otra vez
}

void Aeronave::insertarPuntoRuta(std::size_t indice, const PuntoRuta& p) {
    if (indice > ruta_.size()) throw std::out_of_range("insertarPuntoRuta: indice fuera de rango");
    validarPunto(p);
    ruta_.insert(ruta_.begin() + static_cast<std::ptrdiff_t>(indice), p);
    // Insertar en el pasado conserva el destino actual; insertar en el índice actual lo sustituye.
    if (indice < indiceRuta_) ++indiceRuta_;
    if (estado_ == EstadoAeronave::Llegada && tieneDestino()) estado_ = EstadoAeronave::Volando;
}

void Aeronave::eliminarPuntoRuta(std::size_t indice) {
    if (indice >= ruta_.size()) throw std::out_of_range("eliminarPuntoRuta: indice fuera de rango");
    ruta_.erase(ruta_.begin() + static_cast<std::ptrdiff_t>(indice));
    if (indice < indiceRuta_) --indiceRuta_;
    revisarFinDeRuta();
}

void Aeronave::limpiarRuta() {
    ruta_.clear();
    indiceRuta_ = 0;
}

void Aeronave::setIndiceRuta(std::size_t indice) {
    if (indice > ruta_.size()) throw std::out_of_range("setIndiceRuta: indice fuera de rango");
    indiceRuta_ = indice;
    if (estado_ == EstadoAeronave::Llegada && tieneDestino()) estado_ = EstadoAeronave::Volando;
    revisarFinDeRuta();
}

void Aeronave::revisarFinDeRuta() {
    if (!ruta_.empty() && indiceRuta_ >= ruta_.size() && estaActiva() && modo_ == ModoControl::Ruta)
        estado_ = EstadoAeronave::Llegada;
}

double Aeronave::getAlturaObjetivo() const {
    if (modo_ == ModoControl::Ruta && tieneDestino() && !alturaOrdenada_) return static_cast<double>(ruta_[indiceRuta_].z);
    return alturaObjetivo_;
}

// ---------------------------------------------------------------------------- órdenes

void Aeronave::setVelocidad(double v) {
    if (!(v >= cfg_.velocidadMinima && v <= cfg_.velocidadMaxima))
        throw std::invalid_argument("Velocidad " + std::to_string(v) + " fuera de los limites [" +
                                    std::to_string(cfg_.velocidadMinima) + ", " + std::to_string(cfg_.velocidadMaxima) + "]");
    velocidad_ = v;
}

void Aeronave::setAlturaObjetivo(double h) {
    if (!(h >= cfg_.alturaMinima && h <= cfg_.alturaMaxima))
        throw std::invalid_argument("Altura " + std::to_string(h) + " fuera de los limites [" +
                                    std::to_string(cfg_.alturaMinima) + ", " + std::to_string(cfg_.alturaMaxima) + "]");
    alturaObjetivo_ = h;
    alturaOrdenada_ = true;
}

void Aeronave::setCombustible(double c) {
    if (!(c > 0.0 && c <= cfg_.combustibleMaximo))
        throw std::invalid_argument("Combustible " + std::to_string(c) + " fuera de (0, " +
                                    std::to_string(cfg_.combustibleMaximo) + "]");
    combustible_ = c;
}

// ----------------------------------------------------------------------- geometría

double Aeronave::distanciaHorizontalA(const Posicion& p) const { return std::hypot(p.x - x_, p.y - y_); }

double Aeronave::gradosHacia(const Posicion& p) const {
    const double dx = p.x - x_, dy = p.y - y_;
    if (dx == 0.0 && dy == 0.0) return rumboGrados_;
    return normalizarGrados(std::atan2(dx, dy) * 180.0 / kPi);
}

double Aeronave::getRadioCaptura() const {
    double radio = cfg_.toleranciaLlegadaXY;
    if (cfg_.velocidadGiro > 0.0) {
        const double radioDeGiro = velocidad_ / (cfg_.velocidadGiro * kPi / 180.0);  // r = v / ω
        radio = std::max(radio, 1.05 * radioDeGiro);
    }
    return radio;
}

void Aeronave::avanzar(double rumboGrados, double distancia) {
    const double r = rumboGrados * kPi / 180.0;
    x_ += std::sin(r) * distancia;
    y_ += std::cos(r) * distancia;
}

void Aeronave::avanzar(Direccion d, double distancia) { avanzar(gradosDe(d), distancia); }

void Aeronave::girarHaciaGrados(double objetivoGrados, double maxGrados) {
    const double diff = diferenciaAngular(rumboGrados_, objetivoGrados);
    if (std::fabs(diff) <= maxGrados) {
        rumboGrados_ = normalizarGrados(objetivoGrados);
    } else {
        rumboGrados_ = normalizarGrados(rumboGrados_ + std::copysign(maxGrados, diff));
    }
}

void Aeronave::acercarAltura(double objetivo, double maxDelta) {
    objetivo = std::clamp(objetivo, cfg_.alturaMinima, cfg_.alturaMaxima);
    z_ += std::clamp(objetivo - z_, -maxDelta, maxDelta);
}

// ----------------------------------------------------- lo que usa el Tablero (friend)

bool Aeronave::acotarAlTablero() {
    const double x = std::clamp(x_, 0.0, static_cast<double>(limiteX_ - 1));
    const double y = std::clamp(y_, 0.0, static_cast<double>(limiteY_ - 1));
    const bool fuera = x != x_ || y != y_;
    x_ = x;
    y_ = y;
    z_ = std::clamp(z_, cfg_.alturaMinima, cfg_.alturaMaxima);
    return fuera;
}

void Aeronave::consumirCombustible(double cantidad) {
    combustible_ = std::max(0.0, combustible_ - std::max(0.0, cantidad));
    if (combustible_ <= 0.0 && estaActiva()) estado_ = EstadoAeronave::SinCombustible;
}

bool Aeronave::comprobarLlegada() {
    bool cambio = false;
    while (estaActiva() && modo_ == ModoControl::Ruta && tieneDestino()) {
        const PuntoRuta& p = ruta_[indiceRuta_];
        if (std::hypot(p.x - x_, p.y - y_) > getRadioCaptura()) break;
        // Con una altura ordenada por el ATC, el punto se da por alcanzado aunque no esté a su altura.
        if (!alturaOrdenada_ && std::fabs(p.z - z_) > cfg_.toleranciaLlegadaZ) break;
        ++indiceRuta_;
        cambio = true;
    }
    if (cambio) revisarFinDeRuta();
    return cambio;
}

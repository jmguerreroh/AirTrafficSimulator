#include "CollisionSystem.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>

bool estanEnColision(const Aeronave& a, const Aeronave& b, const ConfiguracionSimulacion& config) {
    const Posicion pa = a.getPosicion(), pb = b.getPosicion();
    const int dx = std::abs(pa.x - pb.x), dy = std::abs(pa.y - pb.y);
    const bool adyacentes = config.colisionEnDiagonales ? std::max(dx, dy) <= 1 : dx + dy <= 1;
    return adyacentes && std::fabs(a.getAltura() - b.getAltura()) <= config.umbralColisionAltura;
}

std::vector<std::pair<std::size_t, std::size_t>> detectarColisiones(
    const std::vector<std::unique_ptr<Aeronave>>& aeronaves, const ConfiguracionSimulacion& config) {
    std::vector<std::pair<std::size_t, std::size_t>> pares;
    for (std::size_t i = 0; i < aeronaves.size(); ++i) {
        const Aeronave& a = *aeronaves[i];
        if (a.getEstado() == EstadoAeronave::Llegada) continue;
        for (std::size_t j = i + 1; j < aeronaves.size(); ++j) {
            const Aeronave& b = *aeronaves[j];
            if (b.getEstado() == EstadoAeronave::Llegada) continue;
            if (a.getEstado() == EstadoAeronave::Colisionada && b.getEstado() == EstadoAeronave::Colisionada) continue;
            if (estanEnColision(a, b, config)) pares.emplace_back(i, j);
        }
    }
    return pares;
}

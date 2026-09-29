// CollisionSystem.h - Detección de colisiones entre aeronaves (lógica pura, sin gráficos).
//
// Dos aeronaves colisionan cuando:
//   1. sus CASILLAS son adyacentes (o la misma): con diagonales, distancia de Chebyshev <= 1;
//      sin diagonales, distancia de Manhattan <= 1;
//   2. |alturaA - alturaB| <= config.umbralColisionAltura.
// Las aeronaves en estado Llegada quedan fuera del tráfico y no colisionan. Los restos
// (Colisionada) y las aeronaves SinCombustible sí son obstáculos. Dos restos entre sí no generan
// colisiones nuevas.
#pragma once

#include <memory>
#include <utility>
#include <vector>

#include "Aeronave.h"
#include "Configuracion.h"

// Pares de índices (i < j) del vector que colisionan ahora mismo.
std::vector<std::pair<std::size_t, std::size_t>> detectarColisiones(
    const std::vector<std::unique_ptr<Aeronave>>& aeronaves, const ConfiguracionSimulacion& config);

// ¿Están A y B en colisión geométrica? (ignora estados)
bool estanEnColision(const Aeronave& a, const Aeronave& b, const ConfiguracionSimulacion& config);

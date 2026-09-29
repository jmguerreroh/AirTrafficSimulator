// AircraftModels.h - Modelos 3D de las aeronaves y de la llama, construidos con primitivas.
//
// Cada modelo es una lista de cubos y pirámides (instancias con su matriz Model y su color).
// Sistema local del modelo: +X = morro (hacia delante), +Y = arriba, +Z = lado derecho.
// El canal alfa del color es la "emisión": 0 = iluminado normalmente, 1 = color plano brillante.
#pragma once

#include <vector>

#include <glm/glm.hpp>

#include "Color.h"
#include "Mesh.h"
#include "Tipos.h"

struct SceneBatches {
    std::vector<InstanceData> cubes;
    std::vector<InstanceData> wedges;  // pirámides (punta hacia +X)
};

// 'frame' lleva el modelo al mundo (posición y rumbo). 'spin' = ángulo de las hélices en radianes.
// 'wreck': restos tras una colisión (oscuros, sin hélices girando). 'dead': sin combustible (apagado).
void appendAircraftModel(SceneBatches& out, TipoAeronave tipo, const glm::mat4& frame, const Color& color,
                         bool wreck, bool dead, float spin);

// Llama de colisión centrada en 'position'. age01 = 0 (recién ocurrida) .. 1 (a punto de apagarse).
// 'scale' agranda la llama en tableros grandes (para que se vea).
void appendExplosion(SceneBatches& out, const glm::vec3& position, float age01, float wallTime, float scale = 1.0f);

// Segmento (cubo alargado) entre dos puntos, con un grosor dado.
InstanceData makeSegment(const glm::vec3& a, const glm::vec3& b, float thickness, const glm::vec4& color);

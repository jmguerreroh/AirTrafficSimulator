// Renderer.h - Traduce un snapshot del Tablero a geometría OpenGL y lo dibuja.
//
// Requiere un contexto OpenGL actual en el hilo que lo usa (lo aporta Window). NO contiene ninguna
// lógica de tráfico aéreo: solo lee el estado (posiciones, rutas, estados...) y lo dibuja.
//
// Correspondencia de ejes (simulación → escena OpenGL):
//     x_gl =  x - (tamX-1)/2        y_gl = altura * kEscalaAltura        z_gl = -(y - (tamY-1)/2)
// (el Norte, +Y de la simulación, queda hacia el fondo de la pantalla en la vista inicial).
//
// Rendimiento: el suelo se reconstruye al llegar un snapshot nuevo (instancias, una llamada de dibujo);
// lo dinámico (aeronaves, rutas, etiquetas, llamas) se reconstruye por frame porque son pocas piezas.
#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include <glm/glm.hpp>

#include "AircraftModels.h"
#include "Camera.h"
#include "Mesh.h"
#include "Shader.h"
#include "Tablero.h"

class Renderer {
public:
    static constexpr float kEscalaAltura = 0.3f;  // unidades de escena por unidad de altura

    Renderer();

    void setSnapshot(std::shared_ptr<const Tablero> snapshot);
    // Selecciona la aeronave bajo el cursor (o ninguna) a partir de un clic en coordenadas NDC.
    void handleClick(const Camera& camera, const glm::vec2& ndc);
    void render(const Camera& camera, const glm::ivec2& framebufferSize, float dt);

private:
    struct Display {  // lo que se dibuja de cada aeronave (suavizado respecto al último snapshot)
        glm::vec3 position{0.0f};
        float headingDegrees = 0.0f;
    };

    glm::vec3 toScene(double x, double y, double z) const;
    void updateDisplays(float dt);
    void rebuildGround();
    void buildDynamicScene(const Camera& camera, SceneBatches& out) const;
    void buildLabels(const Camera& camera, const glm::ivec2& fb, std::vector<InstanceData>& out) const;
    void buildHud(const glm::ivec2& fb, std::vector<InstanceData>& out) const;

    Shader lit_;
    Shader hud_;
    Mesh groundCubes_;
    Mesh dynamicCubes_;
    Mesh dynamicWedges_;
    Mesh labelCubes_;
    Mesh hudCubes_;

    std::shared_ptr<const Tablero> snapshot_;
    std::unordered_map<std::string, Display> displays_;
    std::string selectedId_;
    float wallTime_ = 0.0f;
    float modelScale_ = 1.0f;  // los modelos crecen con el tamaño del tablero para seguir siendo visibles
};

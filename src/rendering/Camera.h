// Camera.h - Cámara orbital en perspectiva.
//
// La cámara mira siempre a un punto (target_) y se sitúa sobre una esfera a su alrededor, definida por
// yaw (giro), pitch (elevación) y distance. Coordenadas de ESCENA (OpenGL): X derecha, Y arriba,
// Z hacia el observador. (El Renderer traduce X,Y,Z de la simulación a estas.)
#pragma once

#include <glm/glm.hpp>

#include "window/Input.h"

class Camera {
public:
    Camera();

    // Encuadra una escena de width × depth (suelo) y 'height' de altura, centrada en el origen,
    // y guarda esa vista como "vista inicial" (tecla R).
    void fitToScene(float width, float depth, float height);
    void reset();
    void setTopView();  // vista cenital (desde arriba)

    void setViewport(int widthPx, int heightPx);
    void update(const Input& input, float dt);

    glm::mat4 view() const;
    glm::mat4 projection() const;
    glm::vec3 position() const;
    glm::vec3 right() const;     // ejes de la cámara en el mundo (para etiquetas "billboard")
    glm::vec3 up() const;
    glm::vec3 backward() const;  // apunta desde la escena hacia el observador
    // Rayo (origen y dirección normalizada) que pasa por un punto de la pantalla en coordenadas NDC.
    void ray(const glm::vec2& ndc, glm::vec3& origin, glm::vec3& direction) const;
    // Tamaño en el mundo de un píxel de pantalla a la distancia 'distance' de la cámara.
    float worldPerPixel(float distance) const;

    float distance() const { return distance_; }
    float pitch() const { return pitch_; }

private:
    struct Pose {
        glm::vec3 target{0.0f};
        float yaw = 0.0f;
        float pitch = 0.0f;
        float distance = 20.0f;
    };

    void applyPose(const Pose& p);
    void pan(const glm::vec2& screenDelta);
    void clampDistance();

    glm::vec3 target_{0.0f};
    float yaw_ = 0.0f;
    float pitch_ = 0.0f;
    float distance_ = 20.0f;
    Pose home_;
    float aspect_ = 4.0f / 3.0f;
    int viewportHeightPx_ = 768;
};

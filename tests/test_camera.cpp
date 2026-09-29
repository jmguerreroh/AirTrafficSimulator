// Cámara y trazado de rayos para la selección con el ratón (sin abrir ventana).
#include <cmath>

#include <glm/glm.hpp>

#include "rendering/Camera.h"
#include "check.h"

int main() {
    Camera small, large;
    small.fitToScene(10, 10, 10);
    large.fitToScene(60, 60, 30);
    CHECK(large.distance() > small.distance());
    CHECK(small.position().y > 0.0f);  // por encima del tablero

    // Todas las esquinas del suelo de la escena grande quedan dentro del cuadro visible.
    const glm::mat4 vp = large.projection() * large.view();
    for (float sx : {-30.0f, 30.0f})
        for (float sz : {-30.0f, 30.0f}) {
            const glm::vec4 clip = vp * glm::vec4(sx, 0.0f, sz, 1.0f);
            CHECK(clip.w > 0.0f);
            CHECK(std::abs(clip.x / clip.w) <= 1.0f && std::abs(clip.y / clip.w) <= 1.0f);
        }

    // El rayo que pasa por un punto proyectado atraviesa ese punto (base de la selección por clic).
    for (const glm::vec3 p : {glm::vec3(3, 5, -4), glm::vec3(-20, 10, 12), glm::vec3(0, 0, 0)}) {
        const glm::vec4 c = large.projection() * large.view() * glm::vec4(p, 1.0f);
        glm::vec3 origin, dir;
        large.ray({c.x / c.w, c.y / c.w}, origin, dir);
        const float t = glm::dot(p - origin, dir);
        CHECK(t > 0.0f);
        CHECK(glm::length(p - (origin + dir * t)) < 1e-2f);
    }

    // Zoom, vista cenital y reset.
    const float home = large.distance();
    Input zoom;
    zoom.scroll = 3.0f;
    large.update(zoom, 0.016f);
    CHECK(large.distance() < home);
    Input top;
    top.topViewRequested = true;
    large.update(top, 0.016f);
    CHECK(large.pitch() > glm::radians(80.0f));
    Input reset;
    reset.resetRequested = true;
    large.update(reset, 0.016f);
    CHECK(std::abs(large.distance() - home) < 1e-4f && large.pitch() < glm::radians(80.0f));

    // La elevación se limita para no dar la vuelta por encima del polo.
    Input orbit;
    orbit.orbiting = true;
    orbit.cursorDelta = {0.0f, 100000.0f};
    large.update(orbit, 0.016f);
    CHECK(large.pitch() < glm::radians(90.0f));
    return 0;
}

#include "AircraftModels.h"

#include <cmath>

#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace {

glm::vec4 rgba(const Color& c, float emission = 0.0f) { return {c.r, c.g, c.b, emission}; }
glm::vec4 rgba(float r, float g, float b, float emission = 0.0f) { return {r, g, b, emission}; }
glm::vec4 mixWhite(const Color& c, float t) {
    return {c.r + (1.0f - c.r) * t, c.g + (1.0f - c.g) * t, c.b + (1.0f - c.b) * t, 0.0f};
}

// Ayudante para colocar piezas en el sistema local del modelo.
struct PartBuilder {
    SceneBatches& out;
    glm::mat4 frame;

    static glm::mat4 local(const glm::vec3& center, const glm::vec3& size, float angle, const glm::vec3& axis) {
        glm::mat4 m = glm::translate(glm::mat4(1.0f), center);
        if (angle != 0.0f) m = glm::rotate(m, angle, axis);
        return glm::scale(m, size);
    }
    void cube(const glm::vec3& c, const glm::vec3& s, const glm::vec4& color, float angle = 0.0f,
              const glm::vec3& axis = {0.0f, 1.0f, 0.0f}) {
        out.cubes.push_back({frame * local(c, s, angle, axis), color});
    }
    void wedge(const glm::vec3& c, const glm::vec3& s, const glm::vec4& color) {
        out.wedges.push_back({frame * local(c, s, 0.0f, {0, 1, 0}), color});
    }
};

}  // namespace

InstanceData makeSegment(const glm::vec3& a, const glm::vec3& b, float thickness, const glm::vec4& color) {
    const glm::vec3 d = b - a;
    const float len = glm::length(d);
    if (len < 1e-5f) {
        return {glm::scale(glm::translate(glm::mat4(1.0f), a), glm::vec3(thickness)), color};
    }
    const glm::vec3 x = d / len;
    const glm::vec3 helper = std::fabs(x.y) > 0.99f ? glm::vec3(1, 0, 0) : glm::vec3(0, 1, 0);
    const glm::vec3 z = glm::normalize(glm::cross(x, helper));
    const glm::vec3 y = glm::cross(z, x);
    const glm::mat4 m(glm::vec4(x * len, 0.0f), glm::vec4(y * thickness, 0.0f), glm::vec4(z * thickness, 0.0f),
                      glm::vec4(0.5f * (a + b), 1.0f));
    return {m, color};
}

void appendAircraftModel(SceneBatches& out, TipoAeronave tipo, const glm::mat4& frame, const Color& color,
                         bool wreck, bool dead, float spin) {
    PartBuilder p{out, frame};

    // Colores: el propio de la aeronave; apagado si no tiene combustible; carbonizado si es un resto.
    Color body = color;
    if (dead) body = {color.r * 0.35f, color.g * 0.35f, color.b * 0.35f};
    if (wreck) body = {0.12f, 0.11f, 0.11f};
    const glm::vec4 main = rgba(body);
    const glm::vec4 light = wreck ? rgba(0.2f, 0.19f, 0.18f) : mixWhite(body, 0.45f);
    const glm::vec4 white = wreck ? rgba(0.16f, 0.15f, 0.15f) : rgba(0.93f, 0.94f, 0.96f);
    const glm::vec4 glass = wreck ? rgba(0.05f, 0.05f, 0.05f) : rgba(0.15f, 0.25f, 0.40f);
    const glm::vec4 dark = rgba(0.12f, 0.12f, 0.14f);
    const float s = wreck || dead ? 0.0f : spin;

    switch (tipo) {
        case TipoAeronave::Avion:
            p.cube({0.00f, 0.00f, 0.00f}, {1.40f, 0.26f, 0.26f}, main);        // fuselaje
            p.wedge({0.85f, 0.00f, 0.00f}, {0.45f, 0.26f, 0.26f}, white);      // morro
            p.cube({0.05f, -0.02f, 0.00f}, {0.42f, 0.05f, 1.90f}, light);      // alas
            p.cube({-0.62f, 0.25f, 0.00f}, {0.30f, 0.42f, 0.05f}, main);       // deriva (cola vertical)
            p.cube({-0.62f, 0.02f, 0.00f}, {0.26f, 0.04f, 0.72f}, light);      // estabilizador
            p.cube({0.35f, 0.16f, 0.00f}, {0.30f, 0.12f, 0.20f}, glass);       // cabina
            break;
        case TipoAeronave::Helicoptero:
            p.cube({0.05f, 0.00f, 0.00f}, {0.80f, 0.42f, 0.46f}, main);        // fuselaje
            p.cube({0.32f, 0.06f, 0.00f}, {0.28f, 0.26f, 0.42f}, glass);       // cristalera
            p.cube({-0.70f, 0.08f, 0.00f}, {0.90f, 0.10f, 0.10f}, light);      // botalón de cola
            p.cube({-1.10f, 0.25f, 0.00f}, {0.12f, 0.32f, 0.05f}, main);       // deriva
            p.cube({0.05f, 0.27f, 0.00f}, {0.07f, 0.14f, 0.07f}, dark);        // mástil
            for (float side : {-0.22f, 0.22f}) {
                p.cube({0.05f, -0.30f, side}, {0.95f, 0.04f, 0.05f}, dark);    // patines
                p.cube({0.22f, -0.20f, side}, {0.04f, 0.22f, 0.04f}, dark);
                p.cube({-0.12f, -0.20f, side}, {0.04f, 0.22f, 0.04f}, dark);
            }
            p.cube({0.05f, 0.36f, 0.00f}, {1.90f, 0.025f, 0.11f}, dark, s);                       // rotor principal
            p.cube({0.05f, 0.36f, 0.00f}, {1.90f, 0.025f, 0.11f}, dark, s + glm::half_pi<float>());
            p.cube({-1.15f, 0.25f, 0.07f}, {0.40f, 0.03f, 0.05f}, dark, s * 1.6f, {0, 0, 1});     // rotor de cola
            break;
        case TipoAeronave::Dron:
            p.cube({0.00f, 0.00f, 0.00f}, {0.36f, 0.14f, 0.36f}, main);        // cuerpo
            p.cube({0.00f, 0.03f, 0.00f}, {1.10f, 0.05f, 0.06f}, dark, glm::quarter_pi<float>());    // brazos
            p.cube({0.00f, 0.03f, 0.00f}, {1.10f, 0.05f, 0.06f}, dark, -glm::quarter_pi<float>());
            p.wedge({0.24f, 0.02f, 0.00f}, {0.16f, 0.08f, 0.12f}, white);      // marca de "delante"
            for (int i = 0; i < 4; ++i) {
                const float sx = (i & 1) ? 0.39f : -0.39f;
                const float sz = (i & 2) ? 0.39f : -0.39f;
                const float dir = (i == 0 || i == 3) ? 1.0f : -1.0f;
                p.cube({sx, 0.07f, sz}, {0.10f, 0.08f, 0.10f}, dark);          // motor
                p.cube({sx, 0.13f, sz}, {0.52f, 0.015f, 0.07f}, rgba(0.85f, 0.87f, 0.92f), dir * s * 2.0f);  // hélice
            }
            break;
    }
}

void appendExplosion(SceneBatches& out, const glm::vec3& position, float age01, float wallTime, float scale) {
    const float envelope = std::pow(1.0f - std::fmin(age01, 1.0f), 0.6f);
    const float grow = 0.5f + 1.1f * std::fmin(age01 * 4.0f, 1.0f);  // crece rápido y luego se apaga
    const float k = grow * envelope * scale;

    // Núcleo incandescente.
    {
        glm::mat4 m = glm::translate(glm::mat4(1.0f), position + glm::vec3(0, 0.5f * k, 0));
        m = glm::rotate(m, wallTime * 3.0f, glm::normalize(glm::vec3(1, 1, 0.3f)));
        m = glm::scale(m, glm::vec3(1.1f * k));
        out.cubes.push_back({m, rgba(1.0f, 0.90f, 0.35f, 1.0f)});
    }
    // Lenguas de fuego (pirámides hacia arriba) que oscilan.
    const glm::vec3 colors[3] = {{1.0f, 0.25f, 0.05f}, {1.0f, 0.55f, 0.08f}, {1.0f, 0.80f, 0.15f}};
    for (int i = 0; i < 7; ++i) {
        const float ang = i * (glm::two_pi<float>() / 7.0f) + wallTime * 1.3f;
        const float flicker = 0.80f + 0.20f * std::sin(wallTime * 14.0f + i * 1.9f);
        const float height = (1.3f + 0.9f * (i % 3)) * k * flicker;
        const float radius = 0.35f * k * (1.0f + 0.4f * (i % 2));
        const glm::vec3 base = position + glm::vec3(radius * std::cos(ang), 0.0f, radius * std::sin(ang));
        glm::mat4 m = glm::translate(glm::mat4(1.0f), base + glm::vec3(0, 0.5f * height, 0));
        m = glm::rotate(m, ang, glm::vec3(0, 1, 0));
        m = glm::rotate(m, glm::half_pi<float>(), glm::vec3(0, 0, 1));  // punta hacia +Y
        m = glm::scale(m, glm::vec3(height, 0.55f * k, 0.55f * k));
        const glm::vec3& c = colors[i % 3];
        out.wedges.push_back({m, rgba(c.x, c.y, c.z, 1.0f)});
    }
    // Chispas que suben y se dispersan.
    for (int i = 0; i < 8; ++i) {
        const float ph = std::fmod(wallTime * 1.5f + i * 0.37f, 1.0f);
        const float ang = i * 0.785f + 0.3f;
        const glm::vec3 p = position + glm::vec3(std::cos(ang) * ph * 1.6f * k, 0.3f * scale + ph * 2.4f * k,
                                                  std::sin(ang) * ph * 1.6f * k);
        glm::mat4 m = glm::translate(glm::mat4(1.0f), p);
        m = glm::scale(m, glm::vec3(0.12f * scale * (1.0f - ph) * (0.5f + envelope)));
        out.cubes.push_back({m, rgba(1.0f, 0.75f, 0.2f, 1.0f)});
    }
}

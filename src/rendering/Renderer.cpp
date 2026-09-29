#include "Renderer.h"

#include <glad/glad.h>

#include <algorithm>
#include <cmath>
#include <cstdio>

#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "BitmapFont.h"
#include "shaders_embedded.h"

namespace {

const glm::vec3 kClearColor{0.07f, 0.08f, 0.11f};
const glm::vec3 kGroundColor{0.19f, 0.21f, 0.25f};
constexpr float kTileSize = 0.94f;
constexpr float kTileThickness = 0.10f;
constexpr float kPickRadius = 1.3f;  // se multiplica por la escala de los modelos
constexpr float kFontPixelsOnScreen = 2.0f;  // tamaño en pantalla de un píxel de la fuente de las etiquetas
constexpr float kSmoothing = 22.0f;          // 1/s: mayor = sigue antes al estado real

glm::vec4 rgba(const Color& c, float emission) { return {c.r, c.g, c.b, emission}; }

std::string format(const char* fmt, double a) {
    char buf[64];
    std::snprintf(buf, sizeof buf, fmt, a);
    return buf;
}

std::string upper(std::string s) {
    for (char& c : s) c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
    return s;
}

}  // namespace

Renderer::Renderer()
    : lit_(shaders::kBasicVert, shaders::kBasicFrag),
      hud_(shaders::kHudVert, shaders::kHudFrag),
      groundCubes_(Mesh::cube()),
      dynamicCubes_(Mesh::cube()),
      dynamicWedges_(Mesh::pyramid()),
      labelCubes_(Mesh::cube()),
      hudCubes_(Mesh::cube()) {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_MULTISAMPLE);
}

glm::vec3 Renderer::toScene(double x, double y, double z) const {
    return {static_cast<float>(x - 0.5 * (snapshot_->getTamX() - 1)), static_cast<float>(z) * kEscalaAltura,
            static_cast<float>(-(y - 0.5 * (snapshot_->getTamY() - 1)))};
}

void Renderer::setSnapshot(std::shared_ptr<const Tablero> snapshot) {
    const bool sameShape = snapshot_ && snapshot_->getTamX() == snapshot->getTamX() &&
                           snapshot_->getTamY() == snapshot->getTamY();
    snapshot_ = std::move(snapshot);
    modelScale_ = std::clamp(static_cast<float>(std::max(snapshot_->getTamX(), snapshot_->getTamY())) / 25.0f, 1.0f, 4.0f);
    if (!sameShape) displays_.clear();
    rebuildGround();
}

void Renderer::rebuildGround() {
    const int w = snapshot_->getTamX(), h = snapshot_->getTamY();
    std::vector<InstanceData> inst;
    inst.reserve(static_cast<std::size_t>(w) * h + 8);

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            const CeldaHistorial& cell = snapshot_->getCelda(x, y);
            const float checker = ((x + y) & 1) ? 0.90f : 1.0f;  // ajedrezado sutil: ayuda a contar casillas
            glm::vec3 color = kGroundColor * checker;
            if (cell.visitada) {
                // La casilla conserva el color de la última aeronave que pasó por ella.
                color = glm::mix(color, glm::vec3(cell.color.r, cell.color.g, cell.color.b), 0.65f) * (0.9f + 0.1f * checker);
            }
            const glm::vec3 c = toScene(x, y, 0.0);
            glm::mat4 m = glm::translate(glm::mat4(1.0f), {c.x, -0.5f * kTileThickness, c.z});
            m = glm::scale(m, {kTileSize, kTileThickness, kTileSize});
            inst.push_back({m, glm::vec4(color, 0.0f)});
        }
    }

    // Marco luminoso alrededor del tablero.
    const float hx = 0.5f * w, hz = 0.5f * h;
    const glm::vec4 frame(0.55f, 0.6f, 0.7f, 1.0f);
    inst.push_back(makeSegment({-hx, 0.02f, -hz}, {hx, 0.02f, -hz}, 0.08f, frame));
    inst.push_back(makeSegment({hx, 0.02f, -hz}, {hx, 0.02f, hz}, 0.08f, frame));
    inst.push_back(makeSegment({hx, 0.02f, hz}, {-hx, 0.02f, hz}, 0.08f, frame));
    inst.push_back(makeSegment({-hx, 0.02f, hz}, {-hx, 0.02f, -hz}, 0.08f, frame));
    groundCubes_.setInstances(inst);
}

void Renderer::updateDisplays(float dt) {
    const float a = 1.0f - std::exp(-kSmoothing * dt);
    std::unordered_map<std::string, Display> next;
    for (const auto& ptr : snapshot_->getAeronaves()) {
        const Aeronave& ac = *ptr;
        const glm::vec3 target = toScene(ac.getX(), ac.getY(), ac.getAltura());
        // Rumbo de simulación (0 = N, horario) → giro alrededor de Y en la escena: 90° - rumbo.
        const float targetHeading = 90.0f - static_cast<float>(ac.getRumboGrados());

        auto it = displays_.find(ac.getId());
        Display d;
        if (it == displays_.end()) {
            d = {target, targetHeading};
        } else {
            d = it->second;
            d.position += (target - d.position) * a;
            const float diff = std::remainder(targetHeading - d.headingDegrees, 360.0f);  // camino más corto
            d.headingDegrees += diff * a;
        }
        next.emplace(ac.getId(), d);
    }
    displays_ = std::move(next);
    if (!selectedId_.empty() && displays_.find(selectedId_) == displays_.end()) selectedId_.clear();
}

void Renderer::handleClick(const Camera& camera, const glm::vec2& ndc) {
    if (!snapshot_) return;
    glm::vec3 origin, dir;
    camera.ray(ndc, origin, dir);

    std::string best;
    float bestT = 1e30f;
    for (const auto& [id, d] : displays_) {
        const glm::vec3 toCenter = d.position - origin;
        const float t = glm::dot(toCenter, dir);
        if (t < 0.0f) continue;
        if (glm::length(toCenter - dir * t) <= kPickRadius * modelScale_ && t < bestT) {
            bestT = t;
            best = id;
        }
    }
    selectedId_ = best;  // clic en vacío = deseleccionar
}

void Renderer::buildDynamicScene(SceneBatches& out) const {
    const glm::vec4 shadow(0.03f, 0.03f, 0.05f, 0.0f);
    const float ms = modelScale_;

    for (const auto& ptr : snapshot_->getAeronaves()) {
        const Aeronave& ac = *ptr;
        const auto it = displays_.find(ac.getId());
        if (it == displays_.end()) continue;
        const Display& d = it->second;
        const EstadoAeronave st = ac.getEstado();
        const bool wreck = st == EstadoAeronave::Colisionada;
        const bool dead = st == EstadoAeronave::SinCombustible;

        // Modelo 3D orientado según el rumbo.
        glm::mat4 frame = glm::translate(glm::mat4(1.0f), d.position);
        frame = glm::rotate(frame, glm::radians(d.headingDegrees), glm::vec3(0, 1, 0));
        frame = glm::scale(frame, glm::vec3(ms));
        appendAircraftModel(out, ac.getTipo(), frame, ac.getColor(), wreck, dead, wallTime_ * 30.0f);

        // Sombra en el suelo y línea vertical: ayudan a leer la altura.
        {
            glm::mat4 m = glm::translate(glm::mat4(1.0f), {d.position.x, 0.02f, d.position.z});
            m = glm::scale(m, {0.9f * ms, 0.03f, 0.9f * ms});
            out.cubes.push_back({m, shadow});
            if (d.position.y > 0.1f)
                out.cubes.push_back(makeSegment({d.position.x, 0.03f, d.position.z}, d.position, 0.035f * ms,
                                                rgba(ac.getColor(), 0.45f)));
        }

        // Selección: flecha blanca que oscila sobre la aeronave.
        if (ac.getId() == selectedId_) {
            const float bob = 0.15f * std::sin(wallTime_ * 5.0f);
            glm::mat4 m = glm::translate(glm::mat4(1.0f), d.position + glm::vec3(0, (1.55f + bob) * ms, 0));
            m = glm::rotate(m, wallTime_ * 2.0f, glm::vec3(0, 1, 0));
            m = glm::rotate(m, -glm::half_pi<float>(), glm::vec3(0, 0, 1));  // punta hacia abajo
            m = glm::scale(m, glm::vec3(0.55f, 0.45f, 0.45f) * ms);
            out.wedges.push_back({m, glm::vec4(1, 1, 1, 1)});
        }

        // Ruta y destino (solo para aeronaves en servicio que siguen su ruta).
        if (!ac.estaActiva() || ac.getModoControl() != ModoControl::Ruta || !ac.tieneDestino()) continue;
        const glm::vec4 pathColor = rgba(ac.getColor(), 0.75f);
        glm::vec3 previous = d.position;
        const auto& route = ac.getRuta();
        for (std::size_t i = ac.getIndiceRuta(); i < route.size(); ++i) {
            const glm::vec3 wp = toScene(route[i].x, route[i].y, route[i].z);
            out.cubes.push_back(makeSegment(previous, wp, 0.04f * ms, pathColor));
            previous = wp;

            const bool current = i == ac.getIndiceRuta();
            if (!current) {  // puntos futuros: cubito
                glm::mat4 m = glm::translate(glm::mat4(1.0f), wp);
                m = glm::scale(m, glm::vec3(0.28f * ms));
                out.cubes.push_back({m, rgba(ac.getColor(), 0.7f)});
                continue;
            }
            // Destino actual: anillo en el aire, poste hasta el suelo y marca en la casilla.
            for (int k = 0; k < 12; ++k) {
                const float ang = k * glm::two_pi<float>() / 12.0f + wallTime_ * 0.8f;
                glm::mat4 m = glm::translate(glm::mat4(1.0f), wp + glm::vec3(0.7f * ms * std::cos(ang), 0, 0.7f * ms * std::sin(ang)));
                m = glm::scale(m, glm::vec3(0.14f * ms));
                out.cubes.push_back({m, rgba(ac.getColor(), 0.9f)});
            }
            out.cubes.push_back(makeSegment({wp.x, 0.03f, wp.z}, wp, 0.03f * ms, rgba(ac.getColor(), 0.4f)));
            glm::mat4 g = glm::translate(glm::mat4(1.0f), {wp.x, 0.03f, wp.z});
            g = glm::scale(g, {0.75f * ms, 0.03f, 0.75f * ms});
            out.cubes.push_back({g, rgba(ac.getColor(), 0.6f)});
        }
    }

    // Llamas de las colisiones aún recientes.
    const double duration = snapshot_->getConfiguracion().duracionExplosion;
    for (const Colision& c : snapshot_->getColisiones()) {
        const double age = snapshot_->getTiempo() - c.tiempo;
        if (age < 0.0 || age > duration) continue;
        appendExplosion(out, toScene(c.x, c.y, c.z), static_cast<float>(age / std::max(duration, 1e-6)), wallTime_, ms);
    }
}

void Renderer::buildLabels(const Camera& camera, const glm::ivec2& fb, std::vector<InstanceData>& out) const {
    (void)fb;
    const glm::vec3 right = camera.right(), up = camera.up(), back = camera.backward();
    const glm::vec3 camPos = camera.position();

    // Un rectángulo de pantalla (en "píxeles de fuente") con esquina superior izquierda en 'origin'.
    auto rect = [&](const glm::vec3& origin, float scale, float px, float py, float pw, float ph, float depth,
                    const glm::vec4& color) {
        const glm::vec3 center = origin + right * ((px + 0.5f * pw) * scale) - up * ((py + 0.5f * ph) * scale) +
                                 back * (depth * scale);
        const glm::mat4 m(glm::vec4(right * (pw * scale), 0.0f), glm::vec4(up * (ph * scale), 0.0f),
                          glm::vec4(back * (0.5f * scale), 0.0f), glm::vec4(center, 1.0f));
        out.push_back({m, color});
    };

    auto label = [&](const std::string& text, const glm::vec3& anchor, const Color& color) {
        const TextPixels t = rasterizeText(upper(text));
        const float scale = camera.worldPerPixel(glm::length(anchor - camPos)) * kFontPixelsOnScreen;
        // Origen: esquina superior izquierda del texto, centrado horizontalmente sobre el ancla.
        const float total = static_cast<float>(t.width + 4);  // + margen y banda de color
        const glm::vec3 origin = anchor - right * (0.5f * total * scale) + up * (0.5f * kGlyphHeight * scale);
        rect(origin, scale, -1.0f, -1.0f, total + 2.0f, kGlyphHeight + 2.0f, 0.0f, {0.03f, 0.03f, 0.05f, 1.0f});  // fondo
        rect(origin, scale, 0.0f, 0.0f, 2.0f, kGlyphHeight, 1.0f, rgba(color, 1.0f));                            // banda de color
        for (const glm::ivec2& p : t.pixels)
            rect(origin, scale, static_cast<float>(p.x + 3), static_cast<float>(p.y), 1.0f, 1.0f, 2.0f, {1, 1, 1, 1});
    };

    for (const auto& ptr : snapshot_->getAeronaves()) {
        const Aeronave& ac = *ptr;
        const auto it = displays_.find(ac.getId());
        if (it == displays_.end()) continue;
        label(ac.getId(), it->second.position + glm::vec3(0, 1.7f * modelScale_, 0), ac.getColor());

        // El destino actual lleva el id de su aeronave (◎ AV-01).
        if (ac.estaActiva() && ac.getModoControl() == ModoControl::Ruta && ac.tieneDestino()) {
            const PuntoRuta& wp = ac.getDestinoActual();
            label(ac.getId(), toScene(wp.x, wp.y, wp.z) + glm::vec3(0, 1.2f * modelScale_, 0), ac.getColor());
        }
    }
}

void Renderer::buildHud(const glm::ivec2& fb, std::vector<InstanceData>& out) const {
    const int k = std::max(2, fb.y / 350);  // píxeles de pantalla por píxel de fuente
    auto rect = [&](float x, float y, float w, float h, const glm::vec4& color) {
        glm::mat4 m = glm::translate(glm::mat4(1.0f), {x + 0.5f * w, y + 0.5f * h, 0.0f});
        out.push_back({glm::scale(m, {w, h, 1.0f}), color});
    };
    auto text = [&](const std::string& s, float x, float y, const glm::vec4& color) {
        for (const glm::ivec2& p : rasterizeText(upper(s)).pixels) rect(x + p.x * k, y + p.y * k, static_cast<float>(k), static_cast<float>(k), color);
    };
    const glm::vec4 white(1, 1, 1, 1), dim(0.7f, 0.75f, 0.85f, 1.0f), panel(0.03f, 0.04f, 0.07f, 0.78f);
    const float line = static_cast<float>(10 * k), pad = static_cast<float>(6 * k);

    // Panel de la aeronave seleccionada.
    if (const Aeronave* ac = selectedId_.empty() ? nullptr : snapshot_->buscar(selectedId_)) {
        std::string destino = "-";
        if (ac->tieneDestino()) {
            const PuntoRuta& d = ac->getDestinoActual();
            destino = "(" + std::to_string(d.x) + ", " + std::to_string(d.y) + ", " + std::to_string(d.z) + ")";
        }
        const std::string lines[] = {
            "ID: " + ac->getId(),
            std::string("TIPO: ") + aTexto(ac->getTipo()),
            "ALTURA: " + format("%.1f", ac->getAltura()),
            "VELOCIDAD: " + format("%.1f", ac->getVelocidad()),
            "COMBUSTIBLE: " + format("%.0f", ac->getPorcentajeCombustible()) + "%",
            "DESTINO: " + destino,
            std::string("ESTADO: ") + aTexto(ac->getEstado()),
            std::string("MODO: ") + aTexto(ac->getModoControl()),
        };
        std::size_t chars = 0;
        for (const auto& l : lines) chars = std::max(chars, l.size());
        const float width = static_cast<float>(chars * kGlyphAdvance * k) + 3 * pad;
        const float height = static_cast<float>(std::size(lines)) * line + 2 * pad;
        rect(10, 10, width, height, panel);
        rect(10, 10, static_cast<float>(2 * k), height, rgba(ac->getColor(), 1.0f));  // banda con su color
        float y = 10 + pad;
        for (const auto& l : lines) {
            text(l, 10 + 2.0f * pad, y, &l == &lines[0] ? rgba(ac->getColor(), 1.0f) : white);
            y += line;
        }
    }

    // Resumen global (arriba a la derecha).
    {
        const std::string s = "T=" + format("%.1f", snapshot_->getTiempo()) + " S   AERONAVES: " +
                              std::to_string(snapshot_->getAeronaves().size()) + "   COLISIONES: " +
                              std::to_string(snapshot_->getColisiones().size());
        const float w = static_cast<float>(s.size() * kGlyphAdvance * k) + 2 * pad;
        const float x = static_cast<float>(fb.x) - w - 10;
        rect(x, 10, w, line + pad, panel);
        text(s, x + pad, 10 + 0.5f * pad, white);
    }

    // Ayuda (abajo a la izquierda).
    {
        const std::string s = "CLIC: SELECCIONAR   ARRASTRAR: GIRAR   DER: DESPLAZAR   RUEDA: ZOOM   R: INICIAL   2: CENITAL";
        const float w = static_cast<float>(s.size() * kGlyphAdvance * k) + 2 * pad;
        rect(10, static_cast<float>(fb.y) - line - pad - 10, w, line + pad, panel);
        text(s, 10 + pad, static_cast<float>(fb.y) - line - 10 - 0.5f * pad, dim);
    }
}

void Renderer::render(const Camera& camera, const glm::ivec2& fb, float dt) {
    wallTime_ += dt;

    glViewport(0, 0, fb.x, fb.y);
    glClearColor(kClearColor.r, kClearColor.g, kClearColor.b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if (!snapshot_) return;

    updateDisplays(dt);

    SceneBatches dynamic;
    buildDynamicScene(dynamic);
    dynamicCubes_.setInstances(dynamic.cubes);
    dynamicWedges_.setInstances(dynamic.wedges);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    lit_.use();
    lit_.setMat4("uView", camera.view());
    lit_.setMat4("uProjection", camera.projection());
    lit_.setVec3("uViewPos", camera.position());
    lit_.setVec3("uLightDir", glm::normalize(glm::vec3(0.4f, 1.0f, 0.6f)));
    groundCubes_.draw();
    dynamicCubes_.draw();
    dynamicWedges_.draw();

    // Etiquetas: se dibujan encima de todo para que siempre se puedan leer.
    std::vector<InstanceData> labels;
    buildLabels(camera, fb, labels);
    labelCubes_.setInstances(labels);
    glDisable(GL_DEPTH_TEST);
    labelCubes_.draw();

    // Panel 2D en pantalla.
    std::vector<InstanceData> hud;
    buildHud(fb, hud);
    hudCubes_.setInstances(hud);
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    hud_.use();
    hud_.setMat4("uProjection", glm::ortho(0.0f, static_cast<float>(fb.x), static_cast<float>(fb.y), 0.0f, -10.0f, 10.0f));
    hudCubes_.draw();
    glDisable(GL_BLEND);
}

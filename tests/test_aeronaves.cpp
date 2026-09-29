// Comprueba la SOLUCIÓN DE REFERENCIA del profesor (solucion_profesor/). Solo se compila cuando
// existe esa carpeta; los alumnos pueden usarlo como referencia de lo que debe ocurrir.
#include <cmath>
#include <random>
#include <stdexcept>

#include "Avion.h"
#include "Dron.h"
#include "Helicoptero.h"
#include "Tablero.h"
#include "check.h"

static void simular(Tablero& t, double segundos, double dt = 0.05) {
    for (double s = 0; s < segundos; s += dt) t.avanzar(dt);
}

int main() {
    // ---- Reglas de movimiento (puedeMoverse / mover) --------------------------------
    {
        Tablero t(60, 60);
        auto& av = t.crear<Avion>("AV-01", Posicion{10, 10, 30}, Direccion::N);
        auto& he = t.crear<Helicoptero>("HE-01", Posicion{30, 10, 10}, Direccion::N);
        auto& dr = t.crear<Dron>("DR-01", Posicion{50, 10, 10}, Direccion::N);
        CHECK(av.puedeMoverse(Direccion::N) && av.puedeMoverse(Direccion::NE) && av.puedeMoverse(Direccion::E));
        CHECK(av.puedeMoverse(Direccion::W));  // 90° a la izquierda: aún permitido
        CHECK(!av.puedeMoverse(Direccion::S) && !av.puedeMoverse(Direccion::SW) && !av.puedeMoverse(Direccion::SE));
        CHECK(he.puedeMoverse(Direccion::S) && he.puedeMoverse(Direccion::W) && !he.puedeMoverse(Direccion::NE));
        for (int d = 0; d < 8; ++d) CHECK(dr.puedeMoverse(static_cast<Direccion>(d)));
        CHECK_THROWS(av.mover(Direccion::S), std::invalid_argument);
        CHECK_THROWS(he.mover(Direccion::SE), std::invalid_argument);
        he.mover(Direccion::E);
        CHECK(he.getModoControl() == ModoControl::Manual);
        he.seguirRuta();
        CHECK(he.getModoControl() == ModoControl::Ruta);
    }

    // ---- El avión sigue su ruta EN ORDEN, sin retroceder ni quedarse quieto -----------
    {
        Tablero t(80, 80);
        auto& av = t.crear<Avion>("AV-01", Posicion{5, 5, 20}, Direccion::E);
        av.agregarPuntoRuta({30, 8, 30});
        av.agregarPuntoRuta({50, 40, 40});
        av.agregarPuntoRuta({20, 60, 25});
        std::size_t indiceAnterior = 0;
        double t0 = 0;
        while (av.getEstado() != EstadoAeronave::Llegada && t0 < 120) {
            t.avanzar(0.05);
            t0 += 0.05;
            CHECK(av.getIndiceRuta() >= indiceAnterior);  // el índice solo avanza
            indiceAnterior = av.getIndiceRuta();
            CHECK(av.getEstado() != EstadoAeronave::Estacionaria);
            CHECK(av.getVelocidad() >= av.getConfiguracion().velocidadMinima);
        }
        CHECK(av.getEstado() == EstadoAeronave::Llegada);
        CHECK(av.getCombustible() < av.getCombustibleMaximo());
    }

    // ---- Un avión alcanza puntos aleatorios a cualquier velocidad legal ---------------
    {
        std::mt19937 rng(42);
        for (int prueba = 0; prueba < 40; ++prueba) {
            Tablero t(60, 60);
            auto& av = t.crear<Avion>("AV-01", Posicion{30, 30, 50}, static_cast<Direccion>(rng() % 8));
            av.setVelocidad(5.0 + (rng() % 1501) / 100.0);
            av.agregarPuntoRuta({static_cast<int>(rng() % 60), static_cast<int>(rng() % 60), 40 + static_cast<int>(rng() % 30)});
            simular(t, 60.0);
            CHECK(av.getEstado() == EstadoAeronave::Llegada);
        }
    }

    // ---- El helicóptero solo se mueve en cardinales y llega ----------------------------
    {
        Tablero t(60, 60);
        auto& he = t.crear<Helicoptero>("HE-01", Posicion{5, 5, 5});
        he.agregarPuntoRuta({25, 15, 20});
        for (int i = 0; i < 4000 && he.getEstado() != EstadoAeronave::Llegada; ++i) {
            const double x0 = he.getX(), y0 = he.getY();
            t.avanzar(0.025);
            const double dx = he.getX() - x0, dy = he.getY() - y0;
            CHECK(std::fabs(dx) < 1e-9 || std::fabs(dy) < 1e-9);  // nunca en diagonal
        }
        CHECK(he.getEstado() == EstadoAeronave::Llegada);
    }

    // ---- Estacionario: no se mueve pero gasta MÁS que volar despacio -------------------
    {
        Tablero t(30, 30);
        auto& quieto = t.crear<Helicoptero>("HE-01", Posicion{5, 5, 10});
        auto& lento = t.crear<Helicoptero>("HE-02", Posicion{20, 20, 10});
        quieto.setVelocidad(0.0);
        lento.setVelocidad(0.5);
        lento.mover(Direccion::E);
        simular(t, 5.0);
        CHECK(quieto.getEstado() == EstadoAeronave::Estacionaria);
        CHECK(quieto.getPosicion() == Posicion({5, 5, 10}));
        const double gastoQuieto = quieto.getCombustibleMaximo() - quieto.getCombustible();
        const double gastoLento = lento.getCombustibleMaximo() - lento.getCombustible();
        CHECK(gastoQuieto > 0.0 && gastoQuieto > gastoLento);
    }

    // ---- El helicóptero cambia de altura sin desplazarse ------------------------------
    {
        Tablero t(30, 30);
        auto& he = t.crear<Helicoptero>("HE-01", Posicion{5, 5, 5});
        he.setVelocidad(0.0);
        he.setAlturaObjetivo(30.0);
        he.seguirRuta();
        he.mover(Direccion::N);  // manual, con velocidad 0
        simular(t, 6.0);
        CHECK(std::fabs(he.getAltura() - 30.0) < 1e-6 && he.getPosicion().x == 5 && he.getPosicion().y == 5);
    }

    // ---- El dron: vuelo libre en diagonal, altura rápida, espera --------------------------
    {
        Tablero t(60, 60);
        auto& dr = t.crear<Dron>("DR-01", Posicion{5, 5, 2});
        dr.agregarPuntoRuta({25, 25, 18});
        simular(t, 10.0);
        CHECK(dr.getEstado() == EstadoAeronave::Llegada);
        CHECK(std::fabs(dr.getAltura() - 18.0) <= 1.0);

        auto& esp = t.crear<Dron>("DR-02", Posicion{40, 40, 10});
        esp.esperarEn({30, 45, 12});
        const double antes = esp.getCombustible();
        for (int i = 0; i < 1200; ++i) {  // 30 s
            t.avanzar(0.025);
            if (i > 400) CHECK(std::hypot(esp.getX() - 30, esp.getY() - 45) <= esp.getConfiguracionDron().radioEspera * 1.6);
        }
        CHECK(esp.enEspera() && esp.getEstado() == EstadoAeronave::Volando);
        CHECK(esp.getCombustible() < antes);
        esp.cancelarEspera();
        CHECK(!esp.enEspera());
        CHECK_THROWS(esp.esperarEn({30, 45, 99}), std::invalid_argument);
    }

    // ---- Combustible: cada tipo a su ritmo y acaba en SinCombustible -------------------
    {
        Tablero t(60, 60);
        auto& av = t.crear<Avion>("AV-01", Posicion{5, 5, 30}, Direccion::E);
        auto& he = t.crear<Helicoptero>("HE-01", Posicion{5, 20, 10});
        auto& dr = t.crear<Dron>("DR-01", Posicion{5, 40, 10});
        for (Aeronave* a : {static_cast<Aeronave*>(&av), static_cast<Aeronave*>(&he), static_cast<Aeronave*>(&dr)})
            a->setCombustible(5.0);
        he.setVelocidad(0.0);
        dr.setVelocidad(0.0);
        simular(t, 15.0);
        CHECK(av.getEstado() == EstadoAeronave::SinCombustible);
        CHECK(he.getEstado() == EstadoAeronave::SinCombustible);
        CHECK(dr.getEstado() == EstadoAeronave::SinCombustible);
        CHECK(av.getCombustible() == 0.0);
        const double x = av.getX();
        simular(t, 1.0);
        CHECK(av.getX() == x);  // inmóvil
    }

    // ---- Colisión provocada por el movimiento + historial de casillas -----------------
    {
        Tablero t(60, 60);
        auto& a = t.crear<Helicoptero>("HE-01", Posicion{10, 20, 10});
        auto& b = t.crear<Dron>("DR-01", Posicion{30, 20, 11});
        a.agregarPuntoRuta({30, 20, 10});
        b.agregarPuntoRuta({10, 20, 11});
        simular(t, 10.0);
        CHECK(a.getEstado() == EstadoAeronave::Colisionada && b.getEstado() == EstadoAeronave::Colisionada);
        CHECK(t.getColisiones().size() == 1);
        // Una casilla recorrida por ambas conserva el color de la última en pasar.
        CHECK(t.getCelda(10, 20).visitada);
        CHECK(t.getCelda(10, 20).idUltimo == "DR-01" || t.getCelda(10, 20).idUltimo == "HE-01");
    }

    // ---- avanzar(dt) es independiente del tamaño de dt -------------------------------
    {
        auto vuelo = [](double dt) {
            Tablero t(60, 60);
            auto& av = t.crear<Avion>("AV-01", Posicion{5, 5, 30}, Direccion::E);
            av.agregarPuntoRuta({50, 30, 30});
            for (double s = 0; s < 3.0 - 1e-9; s += dt) t.avanzar(dt);
            return std::make_pair(av.getX(), av.getY());
        };
        const auto a = vuelo(0.025), b = vuelo(0.5);
        CHECK(std::fabs(a.first - b.first) < 1e-6 && std::fabs(a.second - b.second) < 1e-6);
        CHECK_THROWS(Tablero(5, 5).avanzar(0.0), std::invalid_argument);
    }
    return 0;
}

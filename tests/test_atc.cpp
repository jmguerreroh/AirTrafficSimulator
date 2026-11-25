// test_atc.cpp - Comprueba el ATC: ejecuta las escenas de examples/escenas.h sin ventana y exige que en
// todas haya 0 colisiones, ninguna aeronave fuera del tablero ni sin combustible y que TODAS las que
// tienen ruta lleguen a su destino. Mientras las aeronaves o el ATC no estén completos, FALLA (es lo esperado).
#include <cstdio>

#include "ATC.h"
#include "escenas.h"

int main() {
    int fallos = 0;
    for (const Escena& e : kEscenas) {
        Tablero t(e.tamX, e.tamY);
        e.crear(t);
        ATC atc;
        const double dt = 0.05;
        while (t.getTiempo() < e.segundos) {
            atc.actualizar(t.getAeronaves(), dt);
            t.avanzar(dt);
        }
        int sinCombustible = 0, conRuta = 0, llegadas = 0;
        for (const auto& a : t.getAeronaves()) {
            if (a->getEstado() == EstadoAeronave::SinCombustible) ++sinCombustible;
            if (a->getRuta().empty()) continue;  // p. ej. un dron en espera: no tiene adónde llegar
            ++conRuta;
            if (a->getEstado() == EstadoAeronave::Llegada) ++llegadas;
        }
        const bool ok = t.getColisiones().empty() && t.getSalidas().empty() && sinCombustible == 0 && llegadas == conRuta;
        if (!ok) ++fallos;
        std::printf("[%s] %-12s colisiones: %zu   fuera: %zu   sin combustible: %d   llegadas: %d de %d\n",
                    ok ? " OK " : "FALLO", e.nombre, t.getColisiones().size(), t.getSalidas().size(), sinCombustible,
                    llegadas, conRuta);
        for (const Colision& c : t.getColisiones())
            std::printf("         colision %s - %s en (%.1f, %.1f, %.1f), t=%.1fs\n", c.idA.c_str(), c.idB.c_str(), c.x,
                        c.y, c.z, c.tiempo);
        for (const Salida& f : t.getSalidas())
            std::printf("         %s se sale del tablero en (%.1f, %.1f, %.1f), t=%.1fs\n", f.id.c_str(), f.x, f.y, f.z,
                        f.tiempo);
    }
    if (fallos == 0) std::printf("test_atc: TODAS LAS COMPROBACIONES OK\n");
    return fallos == 0 ? 0 : 1;
}

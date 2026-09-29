// Detección de colisiones (usa helicópteros a velocidad 0: independiente de la lógica del alumno).
#include "Helicoptero.h"
#include "Tablero.h"
#include "check.h"

static Helicoptero& heli(Tablero& t, Posicion p) {
    auto& h = t.crear<Helicoptero>("", p);
    h.setVelocidad(0.0);
    return h;
}

int main() {
    // Adyacentes en horizontal + altura dentro del umbral (2.0) → colisión.
    {
        Tablero t(20, 20);
        auto& a = heli(t, {5, 5, 10});
        auto& b = heli(t, {6, 5, 12});
        t.avanzar(0.1);
        CHECK(a.getEstado() == EstadoAeronave::Colisionada && b.getEstado() == EstadoAeronave::Colisionada);
        CHECK(t.getColisiones().size() == 1);
        CHECK(t.getColisiones()[0].tiempo > 0.0);
        t.avanzar(1.0);
        CHECK(t.getColisiones().size() == 1);  // los restos no generan colisiones nuevas entre sí
    }
    // Diferencia de altura por encima del umbral → no colisionan.
    {
        Tablero t(20, 20);
        auto& a = heli(t, {5, 5, 10});
        auto& b = heli(t, {6, 5, 13});
        t.avanzar(0.1);
        CHECK(a.getEstado() == EstadoAeronave::Estacionaria && b.getEstado() == EstadoAeronave::Estacionaria);
        CHECK(t.getColisiones().empty());
    }
    // Diagonal: cuenta como adyacente por defecto...
    {
        Tablero t(20, 20);
        heli(t, {5, 5, 10});
        heli(t, {6, 6, 10});
        t.avanzar(0.1);
        CHECK(t.getColisiones().size() == 1);
    }
    // ...y no si se desactivan las diagonales.
    {
        ConfiguracionSimulacion c;
        c.colisionEnDiagonales = false;
        Tablero t(20, 20, c);
        heli(t, {5, 5, 10});
        heli(t, {6, 6, 10});
        heli(t, {5, 4, 10});  // cardinal respecto a la primera
        t.avanzar(0.1);
        CHECK(t.getColisiones().size() == 1);  // solo el par cardinal (0, 2): (0,1) es diagonal y (1,2) está a 2 filas
    }
    // Dos casillas de distancia: no.
    {
        Tablero t(20, 20);
        heli(t, {5, 5, 10});
        heli(t, {7, 5, 10});
        t.avanzar(0.1);
        CHECK(t.getColisiones().empty());
    }
    // Umbral configurable.
    {
        ConfiguracionSimulacion c;
        c.umbralColisionAltura = 5.0;
        Tablero t(20, 20, c);
        heli(t, {5, 5, 10});
        heli(t, {5, 6, 15});
        t.avanzar(0.1);
        CHECK(t.getColisiones().size() == 1);
    }
    return 0;
}

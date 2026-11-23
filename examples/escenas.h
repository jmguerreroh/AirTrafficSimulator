// escenas.h - Escenas de tráfico compartidas por los ejemplos y por test_atc (comprobación del ATC).
//
// Cada escena crea sus aeronaves en un Tablero vacío del tamaño indicado. Con un ATC que funcione,
// ninguna debe terminar con colisiones ni con aeronaves sin combustible.
#pragma once

#include <Avion.h>
#include <Dron.h>
#include <Helicoptero.h>
#include <Tablero.h>

struct Escena {
    const char* nombre;
    int tamX, tamY;
    double segundos;  // duración de la simulación
    void (*crear)(Tablero&);
};

// Las tres situaciones de prueba_colisiones: dos choques seguros sin ATC y un cruce a distinta altura.
inline void escenaColisiones(Tablero& t) {
    auto& av1 = t.crear<Avion>("AV-01", Posicion{15, 55, 15}, Direccion::S);  // 1. choque frontal
    av1.setVelocidad(12.0);
    av1.agregarPuntoRuta({15, 5, 15});
    auto& dr1 = t.crear<Dron>("DR-01", Posicion{15, 5, 15});
    dr1.setVelocidad(6.0);
    dr1.agregarPuntoRuta({15, 55, 15});

    auto& he1 = t.crear<Helicoptero>("HE-01", Posicion{35, 30, 10});  // 2. cruce a la misma altura
    he1.agregarPuntoRuta({58, 30, 10});
    auto& dr2 = t.crear<Dron>("DR-02", Posicion{50, 10, 10});
    dr2.setVelocidad(6.0);
    dr2.agregarPuntoRuta({50, 50, 10});

    auto& av2 = t.crear<Avion>("AV-02", Posicion{30, 55, 40}, Direccion::S);  // 3. cruce seguro
    av2.agregarPuntoRuta({30, 5, 40});
    auto& av3 = t.crear<Avion>("AV-03", Posicion{5, 30, 70}, Direccion::E);
    av3.agregarPuntoRuta({28, 30, 70});
    av3.agregarPuntoRuta({58, 30, 70});
}

// La escena de ejemplo_atc.cpp (y del main.cpp de partida): dos aviones de frente a la misma altura.
inline void escenaEjemploAtc(Tablero& t) {
    t.crear<Avion>("AV-01", Posicion{5, 30, 30}, Direccion::E).agregarPuntoRuta({55, 30, 30});
    t.crear<Avion>("AV-02", Posicion{55, 30, 30}, Direccion::W).agregarPuntoRuta({5, 30, 30});
    t.crear<Helicoptero>("HE-01", Posicion{30, 5, 10}).agregarPuntoRuta({30, 55, 10});
    t.crear<Dron>("DR-01", Posicion{10, 50, 5}).agregarPuntoRuta({50, 10, 15});
}

// La escena de prueba_varias: los tres tipos con rutas, alturas, velocidades y combustible distintos.
inline void escenaVarias(Tablero& t) {
    auto& av1 = t.crear<Avion>("AV-01", Posicion{5, 8, 25}, Direccion::E);
    av1.agregarPuntoRuta({55, 8, 25});
    av1.agregarPuntoRuta({55, 55, 35});
    av1.agregarPuntoRuta({8, 40, 30});
    auto& av2 = t.crear<Avion>("AV-02", Posicion{55, 50, 70}, Direccion::W);
    av2.setVelocidad(16.0);
    av2.agregarPuntoRuta({8, 50, 70});
    av2.agregarPuntoRuta({8, 10, 90});
    av2.agregarPuntoRuta({40, 10, 60});
    auto& av3 = t.crear<Avion>("AV-03", Posicion{30, 5, 50}, Direccion::N);
    av3.setVelocidad(8.0);
    av3.setCombustible(400.0);
    av3.agregarPuntoRuta({30, 55, 55});
    av3.agregarPuntoRuta({50, 30, 45});

    auto& he1 = t.crear<Helicoptero>("HE-01", Posicion{10, 15, 8});
    he1.agregarPuntoRuta({25, 30, 12});
    he1.agregarPuntoRuta({10, 45, 20});
    auto& he2 = t.crear<Helicoptero>("HE-02", Posicion{50, 20, 36});
    he2.setVelocidad(3.0);
    he2.agregarPuntoRuta({50, 45, 36});

    auto& dr1 = t.crear<Dron>("DR-01", Posicion{20, 20, 3});
    dr1.setVelocidad(6.0);
    dr1.agregarPuntoRuta({45, 45, 16});
    dr1.agregarPuntoRuta({20, 48, 5});
    auto& dr2 = t.crear<Dron>("DR-02", Posicion{42, 12, 19});
    dr2.setCombustible(120.0);
    dr2.agregarPuntoRuta({12, 26, 19});
    auto& dr3 = t.crear<Dron>("DR-03", Posicion{25, 55, 1});
    dr3.esperarEn({28, 50, 2});
}

// Prueba de estrés: diez aeronaves cuyas rutas se cruzan en el centro del mapa.
inline void escenaEstres(Tablero& t) {
    t.crear<Avion>("AV-01", Posicion{5, 28, 30}, Direccion::E).agregarPuntoRuta({55, 28, 30});
    t.crear<Avion>("AV-02", Posicion{55, 32, 30}, Direccion::W).agregarPuntoRuta({5, 32, 30});
    t.crear<Avion>("AV-03", Posicion{28, 5, 32}, Direccion::N).agregarPuntoRuta({28, 55, 32});
    t.crear<Avion>("AV-04", Posicion{32, 55, 32}, Direccion::S).agregarPuntoRuta({32, 5, 32});
    t.crear<Helicoptero>("HE-01", Posicion{8, 8, 12}).agregarPuntoRuta({52, 52, 12});
    t.crear<Helicoptero>("HE-02", Posicion{52, 8, 12}).agregarPuntoRuta({8, 52, 12});
    t.crear<Helicoptero>("HE-03", Posicion{30, 52, 14}).agregarPuntoRuta({30, 8, 14});
    t.crear<Dron>("DR-01", Posicion{8, 30, 12}).agregarPuntoRuta({52, 30, 12});
    t.crear<Dron>("DR-02", Posicion{30, 8, 12}).agregarPuntoRuta({30, 52, 12});
    t.crear<Dron>("DR-03", Posicion{45, 45, 10}).agregarPuntoRuta({15, 15, 10});
}

inline const Escena kEscenas[] = {
    {"colisiones", 60, 60, 20.0, escenaColisiones},
    {"ejemplo_atc", 60, 60, 60.0, escenaEjemploAtc},
    {"varias", 60, 60, 60.0, escenaVarias},
    {"estres", 60, 60, 90.0, escenaEstres},
};

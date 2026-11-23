// Rutas dinámicas y validaciones de Aeronave/Tablero (no dependen de la lógica del alumno).
#include <stdexcept>

#include "Dron.h"
#include "Helicoptero.h"
#include "Tablero.h"
#include "check.h"

int main() {
    Tablero t(50, 50);
    Dron& d = t.crear<Dron>("DR-01", Posicion{1, 1, 5});

    // Añadir en orden.
    d.agregarPuntoRuta({10, 10, 5});  // A
    d.agregarPuntoRuta({20, 10, 8});  // B
    d.agregarPuntoRuta({30, 10, 8});  // C
    CHECK(d.getRuta().size() == 3 && d.getIndiceRuta() == 0 && d.tieneDestino());
    CHECK(d.getDestinoActual() == Posicion({10, 10, 5}));

    // Ruta original A → B → C ; el ATC la cambia a A → D → E → B → C
    d.insertarPuntoRuta(1, {15, 20, 6});  // D
    d.insertarPuntoRuta(2, {18, 25, 6});  // E
    CHECK(d.getRuta().size() == 5);
    CHECK(d.getRuta()[3] == Posicion({20, 10, 8}));  // B sigue después de E

    // Insertar en el índice actual sustituye el destino actual; en el pasado lo conserva.
    d.setIndiceRuta(2);
    d.insertarPuntoRuta(0, {2, 2, 5});
    CHECK(d.getIndiceRuta() == 3 && d.getDestinoActual() == Posicion({18, 25, 6}));
    d.insertarPuntoRuta(3, {3, 3, 5});
    CHECK(d.getIndiceRuta() == 3 && d.getDestinoActual() == Posicion({3, 3, 5}));

    // Eliminar.
    d.eliminarPuntoRuta(3);
    CHECK(d.getDestinoActual() == Posicion({18, 25, 6}));
    d.eliminarPuntoRuta(0);
    CHECK(d.getIndiceRuta() == 2);
    d.limpiarRuta();
    CHECK(d.getRuta().empty() && !d.tieneDestino());
    CHECK_THROWS(d.getDestinoActual(), std::logic_error);
    CHECK_THROWS(d.eliminarPuntoRuta(0), std::out_of_range);
    CHECK_THROWS(d.insertarPuntoRuta(1, {1, 1, 1}), std::out_of_range);

    // Validaciones de puntos: fuera del tablero y fuera de los límites de altura del tipo.
    CHECK_THROWS(d.agregarPuntoRuta({50, 10, 5}), std::invalid_argument);
    CHECK_THROWS(d.agregarPuntoRuta({-1, 10, 5}), std::invalid_argument);
    CHECK_THROWS(d.agregarPuntoRuta({10, 10, 21}), std::invalid_argument);  // dron: altura máx 20
    CHECK(d.getRuta().empty());

    // Validaciones de creación.
    CHECK_THROWS(t.crear<Dron>("DR-01", Posicion{2, 2, 5}), std::invalid_argument);       // id duplicado
    CHECK_THROWS(t.crear<Helicoptero>("HE-01", Posicion{2, 2, 41}), std::invalid_argument);  // altura fuera de tipo
    CHECK_THROWS(t.crear<Helicoptero>("HE-01", Posicion{60, 2, 5}), std::invalid_argument);  // fuera del tablero
    CHECK_THROWS(t.crear<Dron>("id con espacios", Posicion{2, 2, 5}), std::invalid_argument);
    CHECK_THROWS(Tablero(0, 5), std::invalid_argument);
    CHECK_THROWS(Tablero(5, Tablero::kTamMaximo + 1), std::invalid_argument);
    CHECK_THROWS(t.agregar(nullptr), std::invalid_argument);
    CHECK_THROWS(d.setVelocidad(9.0), std::invalid_argument);  // dron: máx 8
    CHECK_THROWS(d.setCombustible(0.0), std::invalid_argument);
    CHECK(t.getAeronaves().size() == 1);  // los intentos fallidos no dejan basura

    // IDs y colores automáticos, únicos.
    auto& h1 = t.crear<Helicoptero>("", Posicion{5, 5, 5});
    auto& h2 = t.crear<Helicoptero>("", Posicion{7, 5, 5});
    CHECK(h1.getId() == "HE-01" && h2.getId() == "HE-02");
    CHECK(h1.getColor() != h2.getColor() && h1.getColor() != d.getColor());
    Helicoptero rep("HE-09", {9, 9, 9});
    rep.setColor(h1.getColor());
    CHECK_THROWS(t.agregar(std::make_unique<Helicoptero>(rep)), std::invalid_argument);  // color repetido

    // Copia profunda polimórfica.
    Tablero copia = t.clonar();
    CHECK(copia.getAeronaves().size() == 3 && copia.buscar("HE-02")->getTipo() == TipoAeronave::Helicoptero);
    copia.buscar("HE-02")->agregarPuntoRuta({8, 8, 8});
    CHECK(t.buscar("HE-02")->getRuta().empty());  // independientes

    // Altura ordenada: en modo Ruta manda sobre la de los puntos hasta seguirRuta().
    Tablero t3(30, 30);
    auto& ha = t3.crear<Helicoptero>("HE-01", Posicion{2, 2, 5});
    ha.agregarPuntoRuta({20, 2, 12});
    CHECK(!ha.tieneAlturaOrdenada() && ha.getAlturaObjetivo() == 12.0);
    ha.setAlturaObjetivo(30.0);
    CHECK(ha.tieneAlturaOrdenada() && ha.getAlturaObjetivo() == 30.0);
    CHECK(ha.getModoControl() == ModoControl::Ruta);
    ha.seguirRuta();
    CHECK(!ha.tieneAlturaOrdenada() && ha.getAlturaObjetivo() == 12.0);
    CHECK_THROWS(ha.setAlturaObjetivo(41.0), std::invalid_argument);  // helicóptero: altura máx 40
    CHECK(!ha.tieneAlturaOrdenada());

    // Llegada → nuevo punto → vuelve a la vida.
    Tablero t2(20, 20);
    auto& h = t2.crear<Helicoptero>("HE-01", Posicion{2, 2, 5});
    h.agregarPuntoRuta({2, 2, 5});
    t2.avanzar(0.1);
    CHECK(h.getEstado() == EstadoAeronave::Llegada);
    h.agregarPuntoRuta({8, 2, 5});
    CHECK(h.getEstado() == EstadoAeronave::Volando && h.tieneDestino());
    return 0;
}

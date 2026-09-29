// Ejemplo 6 - Las rutas son estructuras dinámicas.
//
// Ruta original:     A → B → C
// A los 4 segundos:  A → D → E → B → C   (se insertan D y E ANTES de B, que aún no se ha visitado)
// A los 14 segundos: se elimina C y se añade F al final.
// Una aeronave que ya ha llegado (Llegada) vuelve a la vida si se le añade un punto.
#include "comun.h"

int main() {
    Simulador simulador;
    Tablero tablero(60, 60);

    auto& dr = tablero.crear<Dron>("DR-01", Posicion{5, 5, 5});
    dr.setVelocidad(6.0);
    const PuntoRuta A{10, 10, 8}, B{50, 10, 8}, C{50, 50, 8};
    dr.agregarPuntoRuta(A);
    dr.agregarPuntoRuta(B);
    dr.agregarPuntoRuta(C);

    auto& av = tablero.crear<Avion>("AV-01", Posicion{5, 30, 30}, Direccion::E);
    av.agregarPuntoRuta({30, 30, 30});

    bool cambio1 = false, cambio2 = false;
    ejecutar(simulador, tablero, 40.0, [&] {
        if (!cambio1 && tablero.getTiempo() > 4.0) {
            // El destino actual es B (aún no visitado): insertar en su posición lo desplaza hacia atrás.
            const std::size_t i = dr.getIndiceRuta();
            dr.insertarPuntoRuta(i, {25, 40, 12});      // D
            dr.insertarPuntoRuta(i + 1, {40, 30, 16});  // E
            std::cout << "ATC: ruta modificada (" << dr.getRuta().size() << " puntos)\n";
            cambio1 = true;
        }
        if (!cambio2 && tablero.getTiempo() > 14.0) {
            dr.eliminarPuntoRuta(dr.getRuta().size() - 1);  // quita C
            dr.agregarPuntoRuta({8, 45, 6});                // F
            av.agregarPuntoRuta({55, 5, 45});               // el avión ya había llegado: vuelve a volar
            std::cout << "ATC: segunda modificacion\n";
            cambio2 = true;
        }
    }, 0.05, 10.0);
    simulador.fin();
    return 0;
}

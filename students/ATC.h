// ATC.h - Plantilla del sistema de control de tráfico aéreo. ESTA ES LA PRÁCTICA.
//
// El simulador NO decide nada por ti. El ATC recibe todas las aeronaves en cada iteración y
// puede, usando SOLO la interfaz polimórfica de Aeronave:
//   * cambiar su ruta (agregarPuntoRuta, insertarPuntoRuta, eliminarPuntoRuta, limpiarRuta),
//   * darles órdenes (mover, setVelocidad, setAlturaObjetivo, seguirRuta),
//   * enviar drones a esperar (necesita un dynamic_cast<Dron*>, o mejor, un diseño que lo evite).
// Añade aquí los atributos y métodos privados que necesites.
#pragma once

#include <memory>
#include <vector>

#include "Aeronave.h"

class ATC {
public:
    // Se llama una vez por iteración, ANTES de Tablero::avanzar(dt).
    void actualizar(std::vector<std::unique_ptr<Aeronave>>& aeronaves, double dt);

private:
    // TODO: IMPLEMENTAR POR EL ALUMNO
    // (prioridades, reservas de altura, listas de emergencia...)
};

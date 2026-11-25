# AirTrafficSimulator — simulador 3D de tráfico aéreo para prácticas de POO

Biblioteca C++17 (OpenGL 3.3 + GLFW + GLAD + GLM) que muestra en 3D, en tiempo real, un espacio aéreo con
**aviones, helicópteros y drones**. Está pensada para una práctica de **programación orientada a objetos**
(herencia, polimorfismo, clases abstractas): el alumno completa las aeronaves y escribe un **ATC**
(control de tráfico aéreo); el simulador se ocupa de todo lo demás (ventana, cámara, render, tablero, rutas,
combustible, colisiones, historial de casillas).

> El simulador **no** resuelve el problema del ATC: solo pone la infraestructura y muestra lo que pasa.

---

Lee [docs/PRACTICA_ATC.md](docs/PRACTICA_ATC.md) (enunciado). No necesitas saber nada de OpenGL, hilos ni shaders.

## Cómo empezar (lo más sencillo)

Crea una carpeta vacía con **un solo fichero**, `CMakeLists.txt`:

```cmake
cmake_minimum_required(VERSION 3.16)
project(MiPractica CXX)
set(CMAKE_CXX_STANDARD 17)

include(FetchContent)
FetchContent_Declare(ATC
  GIT_REPOSITORY https://github.com/jmguerreroh/AirTrafficSimulator.git
  GIT_TAG main)
FetchContent_MakeAvailable(ATC)

atc_practica(EJEMPLOS TESTS)   # EJEMPLOS: programas de prueba; TESTS: test_aeronaves y test_atc
```

Y compila:

```bash
cmake -S . -B build          # la 1ª vez descarga el simulador y CREA EN TU CARPETA los ficheros a completar
cmake --build build
./build/MiATC
```

Tras el primer `cmake` tu carpeta queda así, con todo listo para completar:

```
MiPractica/
├── CMakeLists.txt
├── main.cpp                  ← tu programa principal (aeronaves, rutas, ATC y bucle)
├── include/
│   ├── Avion.h  Helicoptero.h  Dron.h     ← puedes añadirles atributos y métodos
│   └── ATC.h
└── src/
    ├── Avion.cpp  Helicoptero.cpp  Dron.cpp   ← completar actualizar, puedeMoverse, mover, calcularConsumo
    └── ATC.cpp                                    ← tu sistema de control de tráfico aéreo
```
Busca `// TODO: IMPLEMENTAR POR EL ALUMNO` en `src/`.

Esos ficheros son **tuyos**: CMake no los vuelve a tocar. Cada vez que edites algo, repite `cmake --build build`
(solo se recompila tu código). Si no tienes GLFW instalado, CMake lo descarga y compila él solo.


## Requisitos

Compilador de C++17, CMake ≥ 3.16, `git`, una tarjeta gráfica con OpenGL 3.3 y GLFW 3.3+ (opcional: si falta, se descarga).

```bash
# Ubuntu / Debian (una sola vez)
sudo apt install build-essential cmake git libglfw3-dev xorg-dev
```
En Windows: Visual Studio («Desarrollo para el escritorio con C++») y Git.

En Windows: `cmake --build build --config Release` y `build\Release\MiATC.exe`.

> Mientras `Avion`, `Helicoptero` y `Dron` estén sin completar, **las aeronaves no se mueven** (es lo esperado).
> Empieza por ahí (ver [docs/PRACTICA_ATC.md](docs/PRACTICA_ATC.md)). Para comprobar tu trabajo usa `atc_practica(EJEMPLOS TESTS)`: compila los programas de ejemplo y
> `test_aeronaves` y `test_atc` (se ejecutan con `./build/test_aeronaves` y `./build/test_atc`), todos con tu código.

## Tu primer programa

```cpp
#include <Simulador.h>
#include <Avion.h>
#include <Dron.h>
#include <Helicoptero.h>
#include <ATC.h>

int main()
{
    Simulador simulador;
    Tablero tablero(50, 50);                       // 50 x 50 casillas

    // Crear aeronaves (id opcional: "" → AV-01, HE-01...)
    auto& av = tablero.crear<Avion>("AV-01", Posicion{5, 5, 30}, Direccion::E);
    av.agregarPuntoRuta({40, 5, 30});              // ruta: se visita en orden
    av.agregarPuntoRuta({40, 40, 50});

    auto& dr = tablero.crear<Dron>("DR-01", Posicion{10, 40, 5});
    dr.agregarPuntoRuta({30, 10, 15});

    ATC atc;
    const double dt = 0.05;                        // segundos de simulación por iteración
    while (simulador.abierto()) {
        atc.actualizar(tablero.getAeronaves(), dt);   // 1. el ATC decide
        tablero.avanzar(dt);                          // 2. el mundo avanza
        simulador.setTablero(tablero);                // 3. se muestra (no bloquea)
        // (una pausa, p. ej. std::this_thread::sleep_for, para verlo a velocidad normal)
    }
    simulador.fin();
}
```

* `setTablero()` **no bloquea**: copia el estado y sigue. La primera llamada abre la ventana.
* Crea las aeronaves con `tablero.crear<Tipo>(...)`; el tablero las guarda como `std::unique_ptr<Aeronave>`.
* Recorre todas **sin saber de qué tipo son**: `for (auto& a : tablero.getAeronaves()) a->...` (polimorfismo).
* Todos los números de la práctica (velocidades, alturas, consumos, umbral de colisión...) están en
  [include/Configuracion.h](include/Configuracion.h).

## Sistema de coordenadas

```
Z (altura) ↑        Y (Norte) → fondo del mapa
           |       /
           |      /
           +-----→ X (Este)
```
`Posicion{x, y, z}`: `x` = columna (Este), `y` = fila (Norte), `z` = altura. La celda (0, 0) es la esquina
suroeste. Rumbos en grados: 0° = Norte, 90° = Este, 180° = Sur, 270° = Oeste. `Direccion`: N, NE, E, SE, S, SW, W, NW.

## Qué se ve en pantalla

* **Cada aeronave** con su color único y su identificador («AV-01») en una etiqueta.
* **Sombra y línea vertical** hasta el suelo, para leer la altura.
* **Ruta**: línea desde la aeronave hasta su destino actual y los siguientes puntos. El **destino actual** es un
  anillo con el id de su aeronave.
* **Historial**: cada casilla conserva el color de la **última aeronave que pasó** por ella.
* **Colisión**: una **llama 3D** durante unos segundos; los restos quedan oscuros (estado `Colisionada`).
* **Salida del tablero**: la aeronave se estrella en el borde, con la misma llama, y sus restos quedan allí
  (estado `FueraDeLimites`). Son un obstáculo, como los de una colisión.
* **Resumen** (arriba a la derecha): tiempo, número de aeronaves, colisiones (`COLISIONES`) y salidas del
  tablero (`FUERA`).
* **Sin combustible**: aeronave gris e inmóvil (`SinCombustible`), sigue siendo un obstáculo.
* **Clic sobre una aeronave** → panel con ID, tipo, altura, velocidad, combustible, destino, estado y modo.

## Controles

| Acción | Control |
|---|---|
| Seleccionar aeronave | clic izquierdo (sin arrastrar) |
| Girar la cámara | botón izquierdo + arrastrar (o `Q` / `E`) |
| Desplazar la cámara | botón derecho (o central) + arrastrar (o flechas / `WASD`) |
| Zoom | rueda (o `+` / `-`) |
| Vista inicial (perspectiva) | `R` o `1` |
| Vista cenital (desde arriba) | `2` |
| Cerrar | `Esc` o cerrar la ventana |

Guía completa de la API: [docs/API.md](docs/API.md).

## Limitaciones

* **macOS no está soportado** (la ventana se crea en un hilo propio, y macOS exige el hilo principal). Funciona en Linux y Windows.
* Solo puede haber **una ventana** del simulador abierta a la vez por programa.
* Tamaño máximo del tablero: 200 × 200.
* No es un simulador aeronáutico realista: las reglas están simplificadas a propósito con fines docentes.

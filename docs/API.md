<style>
  h1, h2, h3, h4 { page-break-after: avoid; break-after: avoid; }
  pre, blockquote, tr { page-break-inside: avoid; break-inside: avoid; }
  thead { display: table-header-group; }
  p, li { orphans: 3; widows: 3; }
  pre { white-space: pre-wrap; font-size: 0.85em; }
  table { table-layout: fixed; width: 100%; }
  td, th { overflow-wrap: anywhere; word-break: break-word; }
  th:first-child:nth-last-child(2) { width: 42%; }
</style>

# Referencia de la API

Todo lo que se usa del simulador está en su carpeta `include/`, y las tres aeronaves en `Avion.h`, `Helicoptero.h` y `Dron.h`. Los números de las reglas (velocidades, alturas, consumos...) están en `Configuracion.h`.

```cpp
#include <Simulador.h>    // también incluye Tablero, Aeronave, Posicion, Direccion, Tipos y Configuracion
#include <Avion.h>
#include <Helicoptero.h>
#include <Dron.h>
```

Contenido:

1. [Posiciones y direcciones](#posiciones-y-direcciones)
2. [Tipos, estados y modos](#tipos-estados-y-modos)
3. [Aeronave](#aeronave)
4. [Los tres tipos de aeronave](#los-tres-tipos-de-aeronave)
5. [Tablero](#tablero)
6. [Simulador](#simulador)
7. [Configuración](#configuración)
8. [Errores](#errores)

## Posiciones y direcciones

`Posicion.h`, `PuntoRuta.h` y `Direccion.h`.

### Sistema de coordenadas

* `x` crece hacia el **Este** (0 ≤ x < tamX).
* `y` crece hacia el **Norte** (0 ≤ y < tamY).
* `z` es la **altura** (0 ≤ z ≤ 120, el techo del espacio aéreo).
* La casilla (0, 0) es la esquina **suroeste**.

### Posicion y PuntoRuta

```cpp
struct Posicion { int x = 0; int y = 0; int z = 0; };
using PuntoRuta = Posicion;   // un punto de ruta es una posición
```

Son enteros: casilla y altura. Se comparan con `==` y `!=`, y se imprimen con `<<` (`std::cout << p` escribe `(5, 5, 30)`). La posición exacta de una aeronave, con decimales, se lee con `getX()`, `getY()` y `getAltura()`. `getPosicion()` la devuelve redondeada.

### Direccion y rumbos

```cpp
enum class Direccion { N, NE, E, SE, S, SW, W, NW };
```

El **rumbo** es un ángulo en grados que crece en sentido horario, como una brújula: N = 0°, NE = 45°, E = 90°, S = 180° y W = 270°.

| Función | Qué hace |
|---|---|
| `gradosDe(d)` | rumbo de una dirección: `gradosDe(Direccion::S)` es 180 |
| `direccionDesdeGrados(g)` | dirección más cercana a un rumbo: `direccionDesdeGrados(100)` es `E` |
| `normalizarGrados(g)` | lleva un ángulo a [0, 360): `normalizarGrados(-90)` es 270 |
| `diferenciaAngular(origen, destino)` | cuánto girar de un rumbo a otro, en (−180, 180]. Positivo es a la derecha: `diferenciaAngular(90, 180)` es 90 y `diferenciaAngular(0, 270)` es −90 |
| `esCardinal(d)` | `true` para N, E, S y W |
| `deltaX(d)`, `deltaY(d)` | lo que cambian `x` e `y` al dar un paso de una casilla: `deltaX(Direccion::NE)` es 1 |
| `aTexto(d)` | `"N"`, `"NE"`... |

## Tipos, estados y modos

`Tipos.h`.

`TipoAeronave`: `Avion`, `Helicoptero` y `Dron`.

`EstadoAeronave`:

| Estado | Significado | ¿Se mueve? | ¿Puede chocar? |
|---|---|---|---|
| `Volando` | se ha desplazado en el último paso | sí | sí |
| `Estacionaria` | no se ha desplazado (vuelo estacionario) | sí | sí |
| `Llegada` | completó su ruta. Sigue visible y no gasta combustible | no | no, está fuera del tráfico |
| `SinCombustible` | se le acabó el combustible | no | sí, es un **obstáculo** |
| `Colisionada` | ha chocado | no | sí, sus restos son un **obstáculo** |

`estaActiva()` es `true` solo en `Volando` y `Estacionaria`. Una aeronave en `Llegada` que recibe un punto de ruta nuevo vuelve a `Volando`.

`ModoControl`, es decir, quién manda sobre la aeronave:

| Modo | Significado |
|---|---|
| `Ruta` (por defecto) | piloto automático: va hacia los puntos de su ruta en orden (a la altura ordenada, si la hay) |
| `Manual` | tras `mover(d)`: sigue esa dirección y no avanza la ruta hasta que se llame a `seguirRuta()` |
| `Espera` | solo drones, tras `esperarEn(centro)`: da vueltas alrededor de un punto |

`aTexto(...)` convierte cualquiera de los tres a texto: `aTexto(a.getEstado())` devuelve `"Volando"`, `"Llegada"`... `prefijoId(tipo)` da el prefijo de los ids automáticos (`"AV"`, `"HE"`, `"DR"`).

<div style="page-break-before: always;"></div>

## Aeronave

`Aeronave.h`. Es la clase base **abstracta** de las tres aeronaves. El tablero y el ATC solo conocen esta interfaz.

### Consultas

| Método | Devuelve |
|---|---|
| `getId()` | identificador: `"AV-01"`, `"HE-02"`... |
| `getTipo()` | `TipoAeronave` |
| `getPosicion()` | casilla y altura redondeadas (`Posicion`) |
| `getX()`, `getY()`, `getAltura()` | posición exacta |
| `getVelocidad()` | casillas por segundo |
| `getRumbo()`, `getRumboGrados()` | rumbo como `Direccion` (la más cercana) o en grados |
| `getCombustible()`, `getCombustibleMaximo()`, `getPorcentajeCombustible()` | combustible que queda, capacidad y porcentaje (0 a 100) |
| `getEstado()`, `estaActiva()` | estado y si está `Volando` o `Estacionaria` |
| `getModoControl()` | `Ruta`, `Manual` o `Espera` |
| `getAlturaObjetivo()`, `tieneAlturaOrdenada()` | altura hacia la que tiende y si hay una orden de altura vigente (ver [Órdenes](#órdenes)) |
| `getConfiguracion()` | sus límites y consumos (ver [Configuración](#configuración)) |
| `getColor()`, `tieneColor()` | color en la ventana y si ya tiene uno asignado |

### Ruta

Los puntos se visitan **en orden**. `getRuta()` devuelve la ruta completa, incluidos los puntos ya visitados, y `getIndiceRuta()` la posición del destino actual dentro de ella.

| Método | Efecto |
|---|---|
| `agregarPuntoRuta(p)` | añade un punto al final |
| `insertarPuntoRuta(i, p)` | inserta en la posición `i`. Si `i` es el destino actual, el nuevo punto pasa a ser el destino. Si es anterior, el destino no cambia |
| `eliminarPuntoRuta(i)` | elimina el punto `i`. Si es anterior al destino, el destino no cambia |
| `limpiarRuta()` | vacía la ruta |
| `setIndiceRuta(i)` | salta al punto `i`. Con `i` igual al tamaño de la ruta se queda sin destino |
| `tieneDestino()` | si queda algún punto por visitar |
| `getDestinoActual()` | el punto al que va. Lanza `std::logic_error` si no tiene destino |

Los puntos se validan: deben estar dentro del tablero y entre la altura mínima y máxima de esa aeronave. Si no, se lanza `std::invalid_argument`. Un índice fuera de la ruta lanza `std::out_of_range`.

No hace falta cambiar de punto a mano: cuando la aeronave alcanza su destino, el tablero pasa al siguiente y, tras el último, la aeronave queda en `Llegada` (ver [Radio de captura](#radio-de-captura)).

### Órdenes

| Método | Efecto |
|---|---|
| `mover(d)` | pasa a modo `Manual` y va en la dirección `d`. Lanza `std::invalid_argument` si su tipo no la permite |
| `puedeMoverse(d)` | si su tipo permite ahora esa dirección (y está activa) |
| `seguirRuta()` | vuelve al modo `Ruta`, con la altura de sus puntos |
| `setVelocidad(v)` | entre la velocidad mínima y máxima de su tipo |
| `setAlturaObjetivo(z)` | entre la altura mínima y máxima de su tipo |
| `setCombustible(c)` | mayor que 0 y como mucho la capacidad del depósito |

**Altura.** `getAlturaObjetivo()` es la altura hacia la que tiende la aeronave. En modo `Ruta` es la del destino actual, salvo que se haya dado una orden con `setAlturaObjetivo(z)`. Esa orden manda sobre la altura de los puntos: la aeronave sigue su ruta a la altura `z`, y da los puntos por alcanzados aunque no esté a su altura, hasta que se llame a `seguirRuta()`, que le devuelve la altura de sus puntos.

```cpp
b.setAlturaObjetivo(25);   // sube a 25 y sigue su ruta a esa altura
// ... más tarde
b.seguirRuta();            // vuelve a la altura de los puntos de su ruta
```

### Geometría

| Método | Devuelve |
|---|---|
| `distanciaHorizontalA(p)` | distancia en casillas hasta `p`, sin contar la altura |
| `gradosHacia(p)` | rumbo desde la aeronave hasta `p` |
| `getRadioCaptura()` | distancia a la que un punto de ruta se da por alcanzado |

### Copia polimórfica

`clonar()` devuelve un `std::unique_ptr<Aeronave>` con una copia completa de la aeronave, del tipo que sea (`Avion`, `Helicoptero` o `Dron`). La copia es independiente: se puede hacer avanzar llamando a su `actualizar(dt)` para ver dónde estaría sin tocar la original.

```cpp
std::unique_ptr<Aeronave> copia = a.clonar();
copia->actualizar(0.5);    // ¿dónde estaría dentro de medio segundo?
```

### A implementar en cada tipo

```cpp
virtual void   actualizar(double dt) = 0;             // mueve la aeronave dt segundos según su modo
virtual bool   puedeMoverse(Direccion d) const = 0;   // ¿permite su tipo esa dirección ahora?
virtual void   mover(Direccion d) = 0;                // control manual (invalid_argument si !puedeMoverse)
virtual double calcularConsumo(double dt) const = 0;  // combustible gastado en dt segundos
```

`calcularConsumo` solo calcula: es el tablero quien descuenta el combustible.

### Herramientas protegidas

Solo se pueden usar dentro de las clases de las aeronaves, para escribir `actualizar` y `mover`.

| Herramienta | Qué hace |
|---|---|
| `avanzar(rumbo, distancia)` | desplaza la aeronave: `avanzar(90.0, 0.5)` la mueve media casilla al Este. También acepta una `Direccion` |
| `girarHaciaGrados(objetivo, maxGrados)` | gira el rumbo hacia `objetivo` por el lado más corto, como mucho `maxGrados` |
| `fijarRumboGrados(g)` | cambia el rumbo de golpe |
| `acercarAltura(objetivo, maxDelta)` | cambia la altura hacia `objetivo` como mucho `maxDelta`, sin salirse de los límites del tipo |
| `setModoControl(m)` | cambia el modo de control |
| `setRumboOrdenadoGrados(g)`, `getRumboOrdenadoGrados()` | guardan y leen el rumbo pedido con `mover` |
| `validarPunto(p)` | lanza `std::invalid_argument` si `p` no vale para esta aeronave |

<div style="page-break-before: always;"></div>

## Los tres tipos de aeronave

`Avion.h`, `Helicoptero.h` y `Dron.h`. Heredan de `Aeronave`.

```cpp
Avion(id, posicion, rumbo, config = ConfiguracionAvion());                       // el rumbo es obligatorio
Helicoptero(id, posicion, rumbo = Direccion::N, config = ConfiguracionHelicoptero());
Dron(id, posicion, rumbo = Direccion::N, config = ConfiguracionDron());
```

Normalmente se crean con `tablero.crear<Tipo>(...)`, que recibe los mismos argumentos. Cada aeronave guarda su propia copia de la configuración, que se consulta con `getConfiguracion()` (los campos comunes) o con `getConfiguracionAvion()`, `getConfiguracionHelicoptero()` y `getConfiguracionDron()` (con los campos propios de su tipo).

**Espera del dron** (ya implementada):

| Método | Efecto |
|---|---|
| `esperarEn(centro)` | pasa a modo `Espera` y da vueltas alrededor de `centro`, a su altura, con radio `radioEspera` |
| `cancelarEspera()` | vuelve a su ruta (equivale a `seguirRuta()`) |
| `enEspera()`, `getCentroEspera()` | si está esperando y dónde |
| `mantenerEspera(dt)` (protegida) | un paso de la vuelta. El `actualizar` del dron la llama en modo `Espera` |

### Reglas por tipo

| | Avión | Helicóptero | Dron |
|---|---|---|---|
| Dirección | siempre su rumbo. Gira como mucho `velocidadGiro` (120 °/s) | N, E, S y W (no diagonales) | cualquier dirección |
| ¿Puede quedarse quieto? | **no** (velocidad mínima 5) | sí (vuelo estacionario, cuesta combustible) | sí |
| `puedeMoverse(d)` | `d` a 90° o menos de su rumbo | `d` cardinal | siempre |
| Velocidad (casillas/s) | 5 a 20 (empieza a 10) | 0 a 10 (empieza a 5) | 0 a 8 (empieza a 4) |
| Altura | 10 a 100 | 0 a 40 | 0 a 20 |
| Subida y bajada (unidades/s) | 8 | 5 | 12 |
| Combustible máximo | 1000 | 800 | 300 |
| Consumo por segundo | 1 + 0,15·v | 1,5 + 0,2·v. Estacionario: 2,5 | 0,6 + 0,1·v. Estacionario: 0,4 |
| Tolerancia de llegada (horizontal / altura) | 3 / 2 | 1 / 1,5 | 0,6 / 1 |

### Radio de captura

Un punto de ruta se da por alcanzado si la distancia horizontal es como mucho `getRadioCaptura()` y la diferencia de altura como mucho `toleranciaLlegadaZ`. Con una altura ordenada con `setAlturaObjetivo`, la altura no se comprueba.

El radio es la tolerancia horizontal del tipo, salvo en aeronaves con giro limitado (el avión), en las que crece con la velocidad: `max(toleranciaLlegadaXY, 1,05·v/ω)`, con ω la velocidad de giro en radianes por segundo. Un avión rápido no puede «afinar» un punto que queda dentro de su círculo de giro.

<div style="page-break-before: always;"></div>

## Tablero

`Tablero.h`. Es el mundo: contiene las aeronaves y hace avanzar el tiempo.

```cpp
Tablero tablero(60, 60);    // tamX × tamY, de 1 a 200 (Tablero::kTamMaximo)
auto& av = tablero.crear<Avion>("AV-01", Posicion{5, 5, 30}, Direccion::E);
av.agregarPuntoRuta({55, 5, 30});
tablero.avanzar(0.05);      // el mundo avanza 0,05 s
```

| Método | Efecto |
|---|---|
| `crear<Tipo>(argumentos...)` | crea una aeronave, la añade y devuelve una referencia a ella (`Tipo&`) |
| `agregar(std::make_unique<Tipo>(...))` | añade una aeronave ya creada. El tablero pasa a ser su dueño. Devuelve `Aeronave&` |
| `getAeronaves()` | `std::vector<std::unique_ptr<Aeronave>>&` con todas (lo que recibe el ATC) |
| `buscar(id)` | `Aeronave*`, o `nullptr` si no existe |
| `avanzar(dt)` | hace avanzar el mundo `dt` segundos (`dt` > 0) |
| `getTiempo()` | segundos de simulación transcurridos |
| `getColisiones()` | `std::vector<Colision>` con todas las colisiones, en orden |
| `getCelda(x, y)` | `CeldaHistorial` de esa casilla. Lanza `std::out_of_range` si está fuera |
| `posicionValida(p)` | si `p` está dentro del tablero y entre 0 y el techo (120). No mira los límites de cada tipo |
| `getTamX()`, `getTamY()` | dimensiones |
| `getConfiguracion()` | `ConfiguracionSimulacion` del tablero |
| `clonar()` | copia completa e independiente del tablero y sus aeronaves |

`crear` y `agregar` lanzan `std::invalid_argument` si la aeronave es nula, el id está repetido, empieza fuera del tablero, su ruta sale de él o su color ya lo usa otra. El id puede tener como mucho 12 caracteres, y solo letras, números, `-` y `_`. Si el id es `""` se le asigna uno (`AV-01`, `HE-01`, `DR-01`...), y si no tiene color, uno distinto de los demás.

**Un tablero no se puede copiar, pero sí mover.** Pásalo a las funciones por referencia (`Tablero&`). Una función sí puede devolver un tablero que ha creado, porque se mueve en lugar de copiarse. Si necesitas una copia de verdad, usa `clonar()`.

```cpp
struct Colision { std::string idA, idB; double x, y, z; double tiempo; };       // punto medio e instante
struct CeldaHistorial { bool visitada; Color color; std::string idUltimo; };   // última aeronave que pasó
```

### Qué ocurre en `avanzar(dt)`

`dt` se divide en subpasos `h` de como mucho `pasoMaximo` (0,025 s), para que ninguna aeronave «atraviese» a otra sin detectarse. **La simulación depende solo de `dt`, no de los FPS de la ventana.** En cada subpaso, para cada aeronave activa:

1. Se llama a `actualizar(h)` (**tu código**, que la mueve).
2. Se impide que salga del tablero, y su estado pasa a `Volando` o `Estacionaria` según se haya desplazado o no.
3. Se descuenta `calcularConsumo(h)` (**tu código**). Si el combustible se agota, pasa a `SinCombustible`.
4. Si ha alcanzado su destino, pasa al siguiente punto. Si era el último, pasa a `Llegada`.

Después se pinta la casilla con el color de cada aeronave y se detectan las colisiones.

### Colisiones

Dos aeronaves colisionan cuando sus casillas son **adyacentes** (o la misma) y su diferencia de altura es como mucho `umbralColisionAltura` (2,0). Por defecto, adyacentes son las 8 casillas que rodean a una. Las dos pasan a `Colisionada`, se registra una `Colision` y en la ventana aparece una llama unos segundos. Dos restos no generan colisiones nuevas entre sí. Todo esto se configura en `ConfiguracionSimulacion`.

## Simulador

`Simulador.h`. Es la ventana: muestra una copia del tablero y se dibuja en su propio hilo.

```cpp
Simulador simulador;
simulador.setTablero(tablero);          // muestra el estado actual y vuelve enseguida (no bloquea)
simulador.setTablero(tablero, 60, 60);  // igual, comprobando las dimensiones
simulador.abierto();                    // false cuando se ha cerrado la ventana
simulador.esperarCierre();              // espera a que el usuario cierre la ventana
simulador.fin();                        // cierra y libera todo
```

| Situación | Comportamiento |
|---|---|
| Primera llamada a `setTablero` | abre la ventana (tarda unas decenas de milisegundos) |
| `setTablero` repetido | cada llamada sustituye lo que se ve. **Hace una copia** del tablero, que sigue siendo tuyo |
| Llamadas más rápidas que el refresco de la pantalla | se dibuja siempre el último estado completo, nunca un tablero a medias |
| El usuario cierra la ventana | `abierto()` pasa a `false` y `setTablero` deja de hacer nada, sin error |
| `fin()` dos veces | no pasa nada |
| `setTablero` después de `fin()` | se ignora. Para abrir otra ventana, crea otro `Simulador` |
| Se te olvida `fin()` | lo llama el destructor |
| Dimensiones distintas de las del tablero | `std::invalid_argument` |
| No hay pantalla u OpenGL 3.3 | `std::runtime_error` en la primera llamada |

<div style="page-break-before: always;"></div>

## Configuración

`Configuracion.h`. Reúne todos los números de las reglas. Unidades: distancias en casillas, tiempos en segundos y alturas en unidades de altura.

**`ConfiguracionAeronave`**, común a los tres tipos (`ConfiguracionAvion`, `ConfiguracionHelicoptero` y `ConfiguracionDron` heredan de ella y cambian los valores):

| Campo | Significado |
|---|---|
| `velocidadMinima`, `velocidadMaxima` | límites de `setVelocidad` |
| `velocidadInicial` | velocidad al crearla |
| `alturaMinima`, `alturaMaxima` | límites de altura del tipo |
| `velocidadAscenso` | lo que puede subir o bajar por segundo |
| `velocidadGiro` | grados por segundo que puede girar (0 = gira al instante) |
| `combustibleMaximo` | capacidad del depósito |
| `consumoBase`, `consumoPorVelocidad` | consumo por segundo en vuelo: `consumoBase + consumoPorVelocidad · v` |
| `consumoEstacionario` | consumo por segundo sin desplazarse |
| `toleranciaLlegadaXY`, `toleranciaLlegadaZ` | distancia horizontal y diferencia de altura para dar un punto por alcanzado |

`ConfiguracionDron` añade `radioEspera` (3 casillas), el radio de la vuelta en modo `Espera`.

**`ConfiguracionSimulacion`**, las reglas del mundo (se pasa opcionalmente al crear el `Tablero`):

| Campo | Significado |
|---|---|
| `umbralColisionAltura` | diferencia de altura máxima para chocar (2,0) |
| `colisionEnDiagonales` | `true`: adyacentes son las 8 casillas de alrededor. `false`: solo N, E, S y W |
| `duracionExplosion` | segundos que se ve la llama tras un choque (5) |
| `pasoMaximo` | tamaño máximo de los subpasos de `avanzar` (0,025 s) |
| `alturaMaximaTablero` | techo del espacio aéreo (120) |

Se pueden crear aeronaves o tableros «especiales» cambiando una copia de la configuración:

```cpp
ConfiguracionAvion c;
c.velocidadMaxima = 30;
tablero.crear<Avion>("AV-99", Posicion{5, 5, 40}, Direccion::E, c);
```

## Errores

Todo lo que no es válido lanza una excepción estándar con un mensaje que explica el problema (se lee con `e.what()`):

| Excepción | Cuándo |
|---|---|
| `std::invalid_argument` | datos no válidos: alturas, velocidades, combustible, ids, colores, puntos fuera del tablero, una dirección que el tipo no permite en `mover`, `dt` ≤ 0... |
| `std::out_of_range` | índices de ruta fuera de rango, `getCelda` fuera del tablero |
| `std::logic_error` | `getDestinoActual()` sin destino |
| `std::runtime_error` | no se puede abrir la ventana |

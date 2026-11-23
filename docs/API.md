# Referencia de la API

Todo lo que necesita el alumno está en `include/`. Los números de las reglas, en `Configuracion.h`.

## Simulador (la ventana)

```cpp
Simulador simulador;
simulador.setTablero(tablero);                 // muestra el estado; NO bloquea
simulador.setTablero(tablero, 50, 50);         // igual, comprobando las dimensiones
simulador.abierto();                           // false cuando se cerró la ventana
simulador.esperarCierre();                     // bloquea hasta que el usuario cierre la ventana
simulador.fin();                               // cierra y libera todo (se puede llamar varias veces)
```

| Situación | Comportamiento |
|---|---|
| Primera llamada a `setTablero` | abre la ventana (tarda decenas de ms) |
| `setTablero` repetido | cada llamada sustituye el estado mostrado; **hace una copia profunda** del tablero |
| Actualizar más deprisa que ~60 Hz | se dibuja siempre el último estado (coherente); nunca se ve un tablero a medias |
| Ventana cerrada por el usuario | `abierto()` pasa a `false`; `setTablero` deja de hacer nada (sin error) |
| `fin(); fin();` | seguro |
| `fin(); setTablero(...)` | se ignora (el simulador queda cerrado; crea otro `Simulador` si quieres abrir otra ventana) |
| Olvidar `fin()` | el destructor lo llama |
| Dimensiones que no coinciden con el tablero | `std::invalid_argument` |
| No hay pantalla / OpenGL 3.3 | `std::runtime_error` en la primera llamada |

## Tablero (el mundo)

```cpp
Tablero tablero(50, 50);                                   // tamX × tamY (1..200); ConfiguracionSimulacion opcional
auto& av = tablero.crear<Avion>("AV-01", Posicion{5,5,30}, Direccion::E);
Aeronave& a = tablero.agregar(std::make_unique<Dron>("", Posicion{1,1,1}));   // toma posesión
tablero.getAeronaves();       // std::vector<std::unique_ptr<Aeronave>>&  (para el ATC)
tablero.buscar("AV-01");      // Aeronave* o nullptr
tablero.avanzar(dt);          // avanza el mundo dt segundos
tablero.getTiempo();          // segundos de simulación
tablero.getColisiones();      // std::vector<Colision>  (idA, idB, x, y, z, tiempo)
tablero.getCelda(x, y);       // CeldaHistorial: visitada, color y idUltimo de la última aeronave que pasó
tablero.posicionValida(p);
```

`agregar` / `crear` validan y lanzan `std::invalid_argument` si: la aeronave es nula, el id está repetido,
empieza fuera del tablero, su ruta sale del tablero o el color ya lo usa otra. Si el id es `""` se asigna uno
(`AV-01`, `HE-01`, `DR-01`...); si no tiene color se le asigna uno único.

### `avanzar(dt)` — qué ocurre en cada paso

`dt` se subdivide en pasos de como mucho `pasoMaximo` (0,025 s) para que ninguna aeronave «atraviese» otra sin
detectarse. **La simulación depende solo de `dt`, no de los FPS de la ventana.** En cada subpaso, para cada
aeronave activa:

1. `actualizar(h)` (**tu código**: movimiento)
2. se impide salir del tablero; estado `Volando` / `Estacionaria` según se haya desplazado
3. `calcularConsumo(h)` (**tu código**) se descuenta del combustible; si se agota → `SinCombustible`
4. si alcanzó el punto actual de la ruta → siguiente punto; si era el último → `Llegada`

Después: se pinta la casilla con el color de la aeronave y se detectan colisiones.

## Aeronave (clase base abstracta)

Consultas: `getId()`, `getTipo()`, `getColor()`, `getPosicion()` (celda), `getX() getY() getAltura()` (exactas),
`getVelocidad()`, `getCombustible()`, `getCombustibleMaximo()`, `getPorcentajeCombustible()`, `getRumbo()`,
`getRumboGrados()`, `getEstado()`, `getModoControl()`, `estaActiva()`, `getConfiguracion()`.

Ruta (se visita **en orden**; todas validan el punto y lanzan `std::invalid_argument` / `std::out_of_range`):

| Método | Efecto |
|---|---|
| `agregarPuntoRuta(p)` | añade al final |
| `insertarPuntoRuta(i, p)` | inserta en la posición `i`; si `i` es el destino actual, el nuevo punto pasa a ser el destino |
| `eliminarPuntoRuta(i)` | elimina |
| `limpiarRuta()` | vacía la ruta |
| `setIndiceRuta(i)` | salta a otro punto |
| `getRuta()`, `getIndiceRuta()`, `tieneDestino()`, `getDestinoActual()` | consulta |

Una aeronave en `Llegada` que recibe un punto nuevo vuelve a `Volando`.

Órdenes: `setVelocidad(v)` (dentro de sus límites), `setAlturaObjetivo(z)`, `setCombustible(c)`, `seguirRuta()`.

Altura: `getAlturaObjetivo()` es la altura hacia la que tiende la aeronave. En modo `Ruta` es la del destino actual, salvo que se haya dado una orden con `setAlturaObjetivo(z)`. Esa orden manda sobre la altura de los puntos: la aeronave sigue su ruta a la altura `z` (y da los puntos por alcanzados aunque no esté a su altura) hasta que se llame a `seguirRuta()`, que le devuelve la altura de sus puntos. `tieneAlturaOrdenada()` indica si hay una orden de altura vigente.
Geometría: `distanciaHorizontalA(p)`, `gradosHacia(p)`, `getRadioCaptura()`.

**A implementar en cada tipo:**

```cpp
virtual void   actualizar(double dt) = 0;          // mover la aeronave dt segundos
virtual bool   puedeMoverse(Direccion d) const = 0;   // ¿permite su tipo esa dirección?
virtual void   mover(Direccion d) = 0;             // control manual (invalid_argument si !puedeMoverse)
virtual double calcularConsumo(double dt) const = 0;  // combustible gastado en dt
```

**Herramientas protegidas** para escribir `actualizar`: `avanzar(rumbo, distancia)`,
`girarHaciaGrados(objetivo, maxGrados)`, `fijarRumboGrados(g)`, `acercarAltura(objetivo, maxDelta)`,
`setModoControl(m)`, `getRumboOrdenadoGrados()`, `setRumboOrdenadoGrados(g)`.

### Modos de control

| Modo | Significado |
|---|---|
| `Ruta` (por defecto) | piloto automático hacia el destino actual (a la altura ordenada, si la hay) |
| `Manual` | tras `mover(dir)`: sigue esa dirección hasta `seguirRuta()` (no avanza la ruta) |
| `Espera` | solo drones, tras `esperarEn(centro)` |

### Estados

`Volando`, `Estacionaria`, `SinCombustible`, `Llegada`, `Colisionada`. Las tres últimas son «terminales»: la
aeronave deja de moverse. `Llegada` permanece **visible**, gasta 0 combustible y **no colisiona** (fuera del
tráfico). `SinCombustible` y `Colisionada` **siguen siendo obstáculos**.

## Reglas por tipo

| | Avión | Helicóptero | Dron |
|---|---|---|---|
| Dirección | siempre su rumbo; gira ≤ `velocidadGiro` °/s | N, E, S, W (no diagonales) | cualquier dirección |
| Quieto | **no** (velocidad mínima 5) | sí (vuelo estacionario, cuesta combustible) | sí |
| `puedeMoverse(d)` | d a ≤ 90° del rumbo | d cardinal | siempre |
| Velocidad (casillas/s) | 5 – 20 | 0 – 10 | 0 – 8 |
| Altura | 10 – 100 | 0 – 40 | 0 – 20 |
| Ascenso (unid./s) | 8 | 5 | 12 |
| Combustible máx. | 1000 | 800 | 300 |
| Consumo/s | 1 + 0,15·v | 1,5 + 0,2·v; estacionario 2,5 | 0,6 + 0,1·v; estacionario 0,4 |
| Extra | — | — | `esperarEn(centro)` / `cancelarEspera()` |

(Valores por defecto de `Configuracion.h`; cada aeronave guarda su propia copia: `ConfiguracionAvion c; c.velocidadMaxima = 30; Avion a("AV-9", pos, rumbo, c);`)

## Colisiones

Dos aeronaves colisionan cuando **(1)** sus casillas son adyacentes (o la misma) — por defecto con las 8 casillas que
rodean — y **(2)** `|alturaA − alturaB| ≤ umbralColisionAltura` (2,0). Ambas pasan a `Colisionada`, se registra una
`Colision` y aparece una llama 3D unos segundos (`duracionExplosion`). Se configura en `ConfiguracionSimulacion`
(`colisionEnDiagonales`, `umbralColisionAltura`...). Dos restos entre sí no generan colisiones nuevas.

## Radio de captura

Un punto de ruta se da por alcanzado si la distancia horizontal ≤ `getRadioCaptura()` y la diferencia de altura ≤
`toleranciaLlegadaZ`. Para aeronaves con giro limitado el radio crece con la velocidad
(`max(toleranciaLlegadaXY, 1,05·v/ω)`): un avión rápido no puede «afinar» un punto que queda dentro de su círculo de giro.

## Errores

Todo lo que no es válido lanza excepciones estándar con un mensaje claro: `std::invalid_argument` (datos no
válidos: alturas, velocidades, ids, colores, rutas fuera del tablero...), `std::out_of_range` (índices de ruta),
`std::logic_error` (`getDestinoActual()` sin destino), `std::runtime_error` (no se puede abrir la ventana).

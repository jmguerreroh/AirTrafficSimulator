# Práctica: sistema de control de tráfico aéreo (ATC)

## Objetivo

Diseñar e implementar, con **programación orientada a objetos**, un ATC capaz de gestionar el tráfico de distintos
tipos de aeronaves en un espacio aéreo tridimensional, evitando colisiones y emergencias. El simulador dibuja el
mundo, mueve el tiempo, gasta combustible y detecta colisiones; **las decisiones son tuyas**.

Conceptos que se practican: clases abstractas, herencia, polimorfismo, métodos virtuales, encapsulación, composición,
contenedores STL, `std::unique_ptr`, excepciones y diseño de algoritmos.

## Material

* [README](../README.md): compilar y ejecutar. [API](API.md): referencia de clases y reglas.
* `examples/`: 8 programas que enseñan cada funcionalidad (prueba_aviones, _helicopteros, _drones, _colisiones,
  _varias, _rutas, _combustible y ejemplo_atc).
* `include/Configuracion.h`: todos los parámetros de la práctica (solo lectura).
* Los ficheros que CMake copia a tu carpeta: `Avion`, `Helicoptero`, `Dron`, `ATC` y `main.cpp`.

## Parte 1 — Las aeronaves (`src/Avion.cpp`, `src/Helicoptero.cpp`, `src/Dron.cpp`)

Implementa en cada clase, **respetando las reglas** de su cabecera (`include/Avion.h`, etc.):

| Método | Qué debe hacer |
|---|---|
| `puedeMoverse(d)` | decir si el tipo admite esa dirección (y si la aeronave está activa) |
| `mover(d)` | tomar el control manual en esa dirección (o `std::invalid_argument` si no puede) |
| `calcularConsumo(dt)` | combustible gastado en `dt`, según estado, velocidad y tipo |
| `actualizar(dt)` | mover la aeronave `dt` segundos: seguir su ruta (modo `Ruta`), obedecer `mover` (modo `Manual`), esperar (`Espera`, dron), y cambiar la altura |

Reglas (resumen; ver [API](API.md#reglas-por-tipo)):

* **Avión**: siempre avanza en su rumbo, no puede pararse (velocidad mínima), gira con un máximo de grados por segundo,
  sube y baja progresivamente, no invierte el rumbo de golpe.
* **Helicóptero**: solo N/E/S/W; puede estar estacionario (**gasta combustible**); cambia de altura sin desplazarse.
* **Dron**: cualquier dirección, altura rápida, modo espera (`mantenerEspera(dt)` ya implementado).

Te ayudan las herramientas protegidas de `Aeronave` (`avanzar`, `girarHaciaGrados`, `acercarAltura`, `gradosHacia`...).
**Comprobación:** `cmake -S . -B build -DATC_TESTS_ACEPTACION=ON && cmake --build build && cd build && ctest`
(`test_aeronaves` debe pasar) y ejecuta `prueba_aviones`, `prueba_helicopteros` y `prueba_drones`.

Cuestión: ¿por qué `Tablero` y el renderer solo conocen `Aeronave` y no `Avion`, `Helicoptero` o `Dron`?
¿Qué habría que tocar para añadir un cuarto tipo (por ejemplo, un globo)?

## Parte 2 — Rutas dinámicas

Usa `agregarPuntoRuta`, `insertarPuntoRuta`, `eliminarPuntoRuta`, `limpiarRuta` y `setIndiceRuta` desde tu ATC.
Ejemplo pedido: la ruta `A → B → C` pasa durante la simulación a `A → D → E → B → C` (ver `prueba_rutas`).

## Parte 3 — El ATC (`src/ATC.cpp`)

Implementa `ATC::actualizar(aeronaves, dt)`, que se llama **una vez por iteración, antes de `Tablero::avanzar`**.
Debe recorrer las aeronaves **polimórficamente** (sin `dynamic_cast` salvo que lo justifiques) y conseguir, como mínimo:

1. **Controlar los tres tipos** de aeronave con una sola interfaz.
2. **Modificar rutas** dinámicamente (desvíos, esperas, reordenaciones).
3. **Evitar colisiones** entre aeronaves en las escenas `prueba_colisiones` y `ejemplo_atc` (0 colisiones).
4. **Gestionar alturas**: separar aeronaves por niveles, respetando los límites de cada tipo.
5. **Gestionar combustible**: no dejar que una aeronave llegue a 0 si puede evitarse (¿atajos? ¿priorizar?).
6. **Priorizar** aeronaves (por ejemplo, poco combustible o emergencias primero).
7. **Reaccionar a emergencias**: una aeronave con poco combustible, restos que bloquean el paso, `SinCombustible`...
8. **Mantener la seguridad del espacio aéreo** durante toda la simulación con 8 o más aeronaves.

Ideas (no son la solución): reservar cuadrículas o «pasillos» por altura; dejar esperar a los drones con `esperarEn`;
ceder el paso al de menos combustible; anticipar la posición futura `p + v·t` de las demás aeronaves; usar
`getRadioCaptura()` para saber cuándo se completa un punto.

## Parte 4 — Programa principal (`alumno/main.cpp`)

Crea al menos **3 escenarios propios** distintos (varias aeronaves de cada tipo, rutas cruzadas, poco combustible)
y ejecútalos con tu ATC. Muestra por consola un resumen (aeronaves llegadas, colisiones, combustible restante).

## Entrega

* Tus carpetas `src/` e `include/` (con cualquier clase que añadas) y `main.cpp`.
* Breve memoria (1–2 páginas): diseño del ATC, estrategia de evitación de colisiones, casos límite y resultados.
* **No modifiques** el código del simulador (la carpeta que descarga CMake). Sí puedes tocar todos los ficheros que CMake te copia a tu carpeta (`include/`, `src/` y `main.cpp`).

## Criterios de evaluación (orientativos)

| Criterio | Peso |
|---|---|
| Aeronaves: reglas correctas, `test_aeronaves` pasa | 25 % |
| Diseño OO: herencia, polimorfismo, encapsulación, ausencia de duplicación | 20 % |
| ATC: evita colisiones y gestiona combustible/alturas en los escenarios | 30 % |
| Rutas dinámicas y prioridades/emergencias | 10 % |
| Calidad del código, pruebas propias y memoria | 15 % |

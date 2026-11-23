<style>
  h1, h2, h3, h4 { page-break-after: avoid; break-after: avoid; }
  pre, blockquote, tr { page-break-inside: avoid; break-inside: avoid; }
  thead { display: table-header-group; }
  p, li { orphans: 3; widows: 3; }
  pre { white-space: pre-wrap; font-size: 0.85em; }
</style>

# Práctica: sistema de control de tráfico aéreo (ATC)

## Objetivo

Vas a diseñar, con **programación orientada a objetos**, un controlador de tráfico aéreo (ATC) que gestione aviones, helicópteros y drones en un espacio aéreo en 3D, sin colisiones y sin que nadie se quede sin combustible.

El simulador dibuja el mundo, hace avanzar el tiempo, descuenta el combustible y detecta las colisiones. **Las decisiones son tuyas**: cómo se mueve cada aeronave y qué órdenes da el ATC.

Conceptos que vas a practicar: clases abstractas, herencia, polimorfismo, métodos virtuales, encapsulación, composición, contenedores STL, `std::unique_ptr`, excepciones y diseño de algoritmos.

## Cómo empezar

### Requisitos

Compilador de C++17, CMake 3.16 o superior, `git` y una tarjeta gráfica con OpenGL 3.3. En Ubuntu o Debian, una sola vez:

```bash
sudo apt install build-essential cmake git libglfw3-dev xorg-dev
```

Si no tienes GLFW instalado, CMake lo descarga y lo compila él solo. En Windows necesitas Visual Studio («Desarrollo para el escritorio con C++») y Git. macOS no está soportado.

### 1. Crea tu carpeta de trabajo

Crea una carpeta nueva para tu práctica, en cualquier sitio:

```bash
mkdir MiPractica
cd MiPractica
```

Dentro, crea un único fichero llamado `CMakeLists.txt` con este contenido:

```cmake
cmake_minimum_required(VERSION 3.16)
project(MiPractica CXX)
set(CMAKE_CXX_STANDARD 17)

include(FetchContent)
FetchContent_Declare(ATC
  GIT_REPOSITORY https://github.com/jmguerreroh/AirTrafficSimulator.git
  GIT_TAG main)
FetchContent_MakeAvailable(ATC)

atc_practica(EJEMPLOS TESTS)
```

`EJEMPLOS` compila también los programas de ejemplo y `TESTS` las comprobaciones automáticas. Los dos usan **tus** aeronaves, así que te servirán para ver si lo que programas funciona.

### 2. Descarga el simulador y crea las plantillas

Desde esa misma carpeta, ejecuta **una sola vez**:

```bash
cmake -S . -B build
```

CMake descarga el simulador (y GLFW si hace falta) dentro de `build/` y **crea en tu carpeta** los ficheros que vas a completar:

```
MiPractica/
├── CMakeLists.txt
├── main.cpp        ← tu programa: mundo, aeronaves, ATC y bucle
├── include/        ← puedes añadir atributos y métodos
│   ├── Avion.h  Helicoptero.h  Dron.h
│   └── ATC.h
├── src/
│   ├── Avion.cpp  Helicoptero.cpp  Dron.cpp   ← reglas de cada tipo
│   └── ATC.cpp                                ← tu controlador
└── build/          ← descargado y compilado (no se entrega)
```

Esos ficheros son **tuyos**: CMake no los vuelve a tocar aunque repitas el comando. Lo que tienes que programar está marcado con `// TODO: IMPLEMENTAR POR EL ALUMNO`.

### 3. Compila y ejecuta

```bash
cmake --build build      # repítelo cada vez que cambies algo
./build/MiATC            # tu programa (main.cpp)
```

Al repetir `cmake --build build` solo se recompila tu código.

En Windows: `cmake --build build --config Release` y `build\Release\MiATC.exe`.

Se abre una ventana con el espacio aéreo y unas aeronaves **quietas**. Es lo esperado: no se moverán hasta que completes sus clases. Prueba los controles: clic sobre una aeronave para ver sus datos, arrastrar para girar la cámara, rueda para el zoom, `2` para la vista desde arriba y `R` para volver a la inicial.

### 4. Programas para comprobar tu trabajo

Además de `MiATC`, en `build/` tienes:

| Programa | Qué deberías ver cuando tus clases estén bien |
|---|---|
| `prueba_aviones` | dos aviones que giran con suavidad, nunca se paran y suben o bajan poco a poco |
| `prueba_helicopteros` | uno recorre su ruta «en escalera» (nunca en diagonal); otro queda quieto en el aire y su combustible baja |
| `prueba_drones` | uno vuela en línea recta en diagonal cambiando de altura deprisa; otro da vueltas vigilando una zona |
| `prueba_combustible` | las aeronaves se quedan sin combustible en este orden: AV-01, DR-01, HE-01 y HE-02 |
| `prueba_rutas` | una ruta que cambia durante el vuelo |
| `prueba_colisiones` | dos choques mientras tu ATC no haga nada; ninguno cuando funcione |
| `prueba_varias`, `ejemplo_atc` | escenas con varias aeronaves de los tres tipos |
| `test_aeronaves` | comprobación automática de las reglas de los tres tipos |
| `test_atc` | comprobación automática del ATC en cuatro escenas (incluida una de diez aeronaves): 0 colisiones, ninguna aeronave sin combustible y todas llegan a su destino |

Se ejecutan igual que tu programa, por ejemplo `./build/prueba_drones`. Mientras tus clases no estén completas, las aeronaves de estos programas tampoco se moverán.

## Material

* [API](API.md): referencia de las clases y reglas de cada tipo de aeronave. [README](../README.md): sistema de coordenadas, qué se ve en pantalla y controles.
* `include/Configuracion.h` del simulador: todos los números de la práctica (velocidades, alturas, consumos, umbral de colisión). Solo lectura.

## Cómo está organizado el guion

La práctica se hace **por fases**. Cada fase añade una sola cosa nueva, tiene algo que **ver** en la ventana y una forma de **comprobar** que está bien antes de seguir. Al final de cada fase hay una **cuestión** para la memoria.

| Fase | Qué haces | Qué ves |
|---|---|---|
| 1 | Poblar el cielo desde `main.cpp` | aeronaves quietas donde tú digas |
| 2 | Control manual: `puedeMoverse` y `mover` | aeronaves que avanzan en la dirección que ordenas |
| 3 | Piloto automático: seguir la ruta | cada tipo llega a su destino a su manera |
| 4 | Combustible: `calcularConsumo` | el depósito baja y una aeronave puede quedarse sin combustible |
| 5 | Rutas dinámicas | rutas que cambian en mitad del vuelo |
| 6 | El ATC, paso a paso | primero avisa de los peligros, luego los evita |
| 7 | Escenarios propios | tu ATC frente a situaciones difíciles |

<div style="page-break-before: always;"></div>

## Fase 1. Poblar el cielo

Antes de mover nada, aprende a crear el mundo. Mientras no completes las aeronaves, **no se moverán**: es lo esperado y te viene bien para ver con calma dónde aparece cada una.

1. En `main.cpp`, crea varias aeronaves de los tres tipos en **posiciones y alturas distintas**:

   ```cpp
   tablero.crear<Avion>("AV-01", Posicion{5, 5, 30}, Direccion::E);
   tablero.crear<Helicoptero>("HE-01", Posicion{30, 10, 15});
   tablero.crear<Dron>("DR-01", Posicion{50, 50, 5});
   ```

   Ejecuta y localízalas en la ventana. Haz **clic** sobre cada una para ver su panel (tipo, altura, combustible...). Usa la vista cenital (tecla `2`) para comprobar las coordenadas: `x` crece hacia el Este y `y` hacia el Norte.
2. Cambia las alturas y observa la **sombra y la línea vertical**: así se lee la altura en 3D.
3. Provoca errores a propósito: una aeronave fuera del tablero, dos con el mismo id, un avión a una altura que no admite. Cada caso lanza una excepción. Captúrala con `try`/`catch` y muestra el mensaje con `e.what()`.
4. Recorre todas las aeronaves e imprime por consola su id, su tipo y su altura:

   ```cpp
   for (const auto& a : tablero.getAeronaves())
       std::cout << a->getId() << " " << aTexto(a->getTipo())
                 << " " << a->getAltura() << "\n";
   ```

**Comprobación:** las aeronaves aparecen donde las has puesto y los errores se informan sin que el programa termine de golpe.

**Cuestión:** `getAeronaves()` devuelve un `std::vector<std::unique_ptr<Aeronave>>`. ¿Por qué el vector guarda punteros a `Aeronave` y no objetos `Avion`, `Helicoptero` o `Dron`? ¿Quién es el dueño de cada aeronave?

<div style="page-break-before: always;"></div>

## Fase 2. Control manual

Empieza por lo más sencillo: que cada aeronave sepa **hacia dónde puede ir** y obedezca una orden de dirección.

1. Implementa `puedeMoverse(d)` en los tres tipos, siguiendo las reglas de cada cabecera:
   * **Avión:** solo direcciones a 90° o menos de su rumbo actual (no puede dar media vuelta de golpe).
   * **Helicóptero:** solo N, E, S y W.
   * **Dron:** cualquier dirección.

   En los tres casos, solo si la aeronave está activa. Te ayudan `diferenciaAngular` y `esCardinal` (`Direccion.h`).
2. Implementa `mover(d)`: si no puede ir en esa dirección, lanza `std::invalid_argument`. Si puede, pasa a modo `Manual` y guarda el rumbo pedido.
3. Implementa **solo la parte manual** de `actualizar(dt)`: en modo `Manual`, avanzar en el rumbo ordenado `getVelocidad() * dt` casillas con `avanzar(...)`. En el avión, gira poco a poco hacia ese rumbo con `girarHaciaGrados(...)` antes de avanzar. El resto de `actualizar` lo completarás en la fase 3.
4. En `main.cpp`, da órdenes y observa:
   * `he.mover(Direccion::N)`: el helicóptero avanza hacia el Norte y pinta las casillas de su color.
   * `av.mover(Direccion::W)` con el avión mirando al Este: debe lanzar una excepción, porque el Oeste queda justo detrás (180°). Con `Direccion::S` (90°, en perpendicular) sí puede: el avión gira suavemente hasta ir hacia el Sur.
   * Haz que un helicóptero **dibuje un cuadrado**: cambia su dirección cada pocos segundos usando `tablero.getTiempo()` dentro del bucle.

**Comprobación:** las órdenes válidas mueven la aeronave y las imposibles lanzan la excepción. El historial de casillas muestra el recorrido.

**Cuestión:** `mover` es un método virtual puro de `Aeronave`. Cuando escribes `aeronave->mover(d)` sobre un puntero a `Aeronave`, ¿cómo sabe el programa qué versión ejecutar? ¿Qué pasaría si `mover` no fuera virtual?

<div style="page-break-before: always;"></div>

## Fase 3. Piloto automático: seguir la ruta

Ahora cada aeronave debe ir sola hacia los puntos de su ruta (modo `Ruta`). Te recomendamos este orden, de más fácil a más difícil:

1. **Dron:** va en línea recta hacia el destino, en cualquier dirección, y cambia de altura deprisa.
2. **Helicóptero:** solo se mueve en N, E, S y W, así que llega «en escalera» (primero un eje y luego el otro). Sin destino, se queda en vuelo estacionario. Puede subir o bajar sin desplazarse.
3. **Avión:** nunca se para. Gira hacia el destino como mucho `velocidadGiro` grados por segundo y avanza siempre en su rumbo. Sube y baja de forma progresiva.

En los tres, la altura se acerca poco a poco a `getAlturaObjetivo()` con `acercarAltura(...)`. Dale a cada aeronave de tu `main.cpp` una ruta de dos o tres puntos con `agregarPuntoRuta` y comprueba que la recorre.

**Comprobación:** ejecuta `./build/prueba_drones`, `./build/prueba_helicopteros` y `./build/prueba_aviones`, y compara lo que ves con lo que describe la tabla de «Programas para comprobar tu trabajo». Después ejecuta `./build/test_aeronaves`: comprueba las reglas de los tres tipos (direcciones, que el avión no se para, que el helicóptero no va en diagonal...). Si algo falla, te dice el fichero y la línea de la comprobación; cuando todo está bien, termina con `TODAS LAS COMPROBACIONES OK`.

**Cuestión:** los tres tipos hacen cosas parecidas (seguir la ruta, cambiar la altura). ¿Qué código has repetido? ¿Podría ir en la clase base o en un método común? ¿Qué ventaja tiene que `Tablero` y la ventana solo conozcan `Aeronave`? ¿Qué tendrías que tocar para añadir un cuarto tipo, por ejemplo un globo?

<div style="page-break-before: always;"></div>

## Fase 4. Combustible

1. Implementa `calcularConsumo(dt)` en los tres tipos, con las fórmulas de [la API](API.md#reglas-por-tipo):
   * el avión siempre gasta (consumo base más un término proporcional a la velocidad);
   * el helicóptero gasta **también cuando está quieto** (vuelo estacionario);
   * el dron gasta poco, pero su depósito es pequeño.
2. Pon a una aeronave poco combustible con `setCombustible(...)` y observa cómo pasa a `SinCombustible`: queda inmóvil, en gris, y **sigue siendo un obstáculo** para las demás.

**Comprobación:** en `prueba_combustible`, las aeronaves se quedan sin combustible en este orden: AV-01, DR-01, HE-01 (en vuelo estacionario) y, por último, HE-02 (que vuela despacio).

**Cuestión:** un helicóptero en espera gasta más que uno que vuela despacio. ¿Qué consecuencias tiene esto para el ATC cuando tenga que hacer esperar a alguien?

## Fase 5. Rutas dinámicas

Una ruta no es fija: se puede cambiar durante el vuelo con `agregarPuntoRuta`, `insertarPuntoRuta`, `eliminarPuntoRuta`, `limpiarRuta` y `setIndiceRuta`. Practica desde `main.cpp`, dentro del bucle, antes de usarlo en el ATC. Se piden estos tres casos:

1. **Patrulla:** un helicóptero recorre los cuatro vértices de un cuadrado y, al llegar al último, vuelve a empezar sin detenerse, durante toda la simulación.
2. **Vuelta a base:** cuando a un dron le quede menos de la mitad del combustible, abandona su ruta y vuelve a su punto de partida, que pasa a ser su único destino.
3. **Desvío:** a los 10 segundos, una aeronave con ruta `A → B → C` debe pasar por un punto `D` entre `B` y `C`, pero solo si todavía no ha llegado a `B`. Si ya lo ha pasado, `D` se añade al final.

**Comprobación:** la línea de ruta de la ventana muestra los cambios al momento. Imprime también la ruta por consola (`getRuta()` y `getIndiceRuta()`) antes y después de cada cambio.

**Cuestión:** ¿qué contenedor usarías para guardar una ruta en la que se insertan y eliminan puntos con frecuencia? ¿Por qué?

## Fase 6. El ATC, paso a paso

`ATC::actualizar(aeronaves, dt)` se llama **una vez por iteración, antes de `Tablero::avanzar`**. Debe recorrer las aeronaves de forma **polimórfica**, sin saber de qué tipo es cada una (sin `dynamic_cast`, salvo que lo justifiques).

No intentes resolverlo todo de golpe. Construye el ATC en capas y, en cada una, comprueba que no has roto la anterior.

### 6.1 Primero, observar

Tu primer ATC **no da ninguna orden**: solo vigila e informa.

* Recorre todas las parejas de aeronaves activas y calcula su distancia horizontal y su diferencia de altura.
* Si están cerca, imprime un aviso: `AVISO t=12.3s: AV-01 y AV-02 a 4 casillas y 0.5 de altura`.
* Usa como referencia la regla de colisión (casillas adyacentes y menos de `umbralColisionAltura` de diferencia de altura) y deja un **margen de seguridad** mayor para avisar con tiempo.

Prueba con la escena de `main.cpp` (dos aviones de frente a la misma altura): el ATC debe avisar **antes** de que choquen.

### 6.2 Separar por altura

La forma más sencilla de evitar un choque es que las aeronaves no estén a la misma altura. Cuando detectes un peligro, ordena a una de las dos que suba o baje con `setAlturaObjetivo(...)`. La aeronave sigue su ruta a esa altura hasta que el peligro haya pasado y le devuelvas la altura de sus puntos con `seguirRuta()`.

* ¿A cuál de las dos? Decide un criterio (por ejemplo, la que tenga más combustible o la que pueda subir más).
* Respeta los límites de altura de cada tipo: un dron no pasa de 20 y un helicóptero de 40.
* ¿Cuándo le devuelves su altura? Si lo haces demasiado pronto, el choque vuelve a estar ahí.

**Objetivo:** 0 colisiones con los dos aviones de frente.

### 6.3 Anticipar

Detectar el peligro cuando ya están cerca puede ser tarde: un avión no se para y sube despacio. Estima dónde estará cada aeronave dentro de unos segundos (posición actual más velocidad por tiempo, en la dirección de su rumbo) y actúa si las posiciones futuras se acercan.

Si no hay altura disponible para separar a dos aeronaves, usa otras herramientas:

* **desvíos:** insertar un punto intermedio en la ruta con `insertarPuntoRuta`;
* **velocidad:** frenar a una de las dos con `setVelocidad` (el avión tiene una velocidad mínima);
* **esperas:** un dron puede quedarse en espera con `esperarEn` y volver a su ruta con `cancelarEspera`.

**Objetivo:** 0 colisiones en `prueba_colisiones` y en `ejemplo_atc`.

### 6.4 Combustible y prioridades

* Si una aeronave va a quedarse sin combustible antes de llegar, dale **prioridad**: que sean las demás las que se aparten, o acórtale la ruta.
* Ordena las aeronaves por urgencia (por ejemplo, menos combustible primero) y resuelve los conflictos en ese orden.

### 6.5 Emergencias y obstáculos

Las aeronaves `SinCombustible` y `Colisionada` se quedan quietas y **siguen siendo obstáculos**. Tu ATC debe desviar a quien vaya a pasar por encima de ellas.

<div style="page-break-before: always;"></div>

### 6.6 Prueba de estrés

Monta una escena con **8 o más aeronaves** de los tres tipos, con rutas que se crucen y alguna con poco combustible. Tu ATC debe mantener la seguridad durante toda la simulación.

**Comprobación:** `./build/test_atc` ejecuta sin ventana cuatro escenas (las de `prueba_colisiones`, `ejemplo_atc` y `prueba_varias`, y una de diez aeronaves que se cruzan en el centro) y exige en todas 0 colisiones, ninguna aeronave sin combustible y que todas las que tienen ruta lleguen a su destino. Cuando todo está bien, termina con `TODAS LAS COMPROBACIONES OK`.

**Requisitos mínimos del ATC:**

1. Controlar los tres tipos con una sola interfaz.
2. Modificar rutas dinámicamente (desvíos, esperas, reordenaciones).
3. Evitar colisiones en `prueba_colisiones` y `ejemplo_atc` (0 colisiones).
4. Separar aeronaves por altura respetando los límites de cada tipo.
5. Evitar que una aeronave se quede sin combustible si se puede.
6. Priorizar aeronaves (poco combustible o emergencias primero).
7. Reaccionar a emergencias y obstáculos.
8. Mantener la seguridad con 8 o más aeronaves.

**Cuestión:** para mandar a un dron a esperar necesitas `esperarEn`, que solo tiene `Dron`. ¿Cómo lo resolverías sin `dynamic_cast`? ¿Qué método podrías añadir a la interfaz común para que el ATC no necesite saber el tipo?

## Fase 7. Escenarios propios

En `main.cpp`, crea al menos **3 escenarios** distintos y ejecútalos con tu ATC. Algunas ideas:

* **Cruce:** cuatro aeronaves que llegan a la vez al centro del mapa desde los cuatro puntos cardinales.
* **Persecución:** un avión rápido que alcanza por detrás a uno lento a la misma altura.
* **Reserva:** varias aeronaves con poco combustible que compiten por pasar.
* **Obstáculo:** una aeronave sin combustible en mitad de una ruta muy transitada.

Al terminar cada escenario, muestra por consola un resumen: aeronaves que han llegado, colisiones y combustible restante.

<div style="page-break-before: always;"></div>

## Entrega

* Tus carpetas `src/` e `include/` (con cualquier clase que añadas) y `main.cpp`.
* Una memoria breve (1 o 2 páginas) con el diseño del ATC, la estrategia para evitar colisiones, los casos límite, los resultados de tus escenarios y las respuestas a las cuestiones de cada fase.
* **No modifiques** el código del simulador (la carpeta que descarga CMake). Sí puedes cambiar todos los ficheros que CMake copia a tu carpeta (`include/`, `src/` y `main.cpp`).

## Criterios de evaluación

| Criterio | Cómo se comprueba | Peso |
|---|---|---|
| Aeronaves (fases 2 a 4) | `test_aeronaves` pasa y los programas de prueba se comportan como describe la tabla de «Programas para comprobar tu trabajo» | 25 % |
| Diseño orientado a objetos | herencia y polimorfismo bien usados, atributos privados, sin código duplicado entre los tres tipos y sin `dynamic_cast` injustificados | 20 % |
| ATC: seguridad (fase 6.1 a 6.3 y 6.6) | `test_atc` pasa y tu escena de 8 o más aeronaves termina sin colisiones | 30 % |
| Rutas, combustible y emergencias (fases 5, 6.4 y 6.5) | los tres casos de la fase 5; ninguna aeronave sin combustible si podía evitarse; prioridades y obstáculos bien resueltos | 10 % |
| Calidad, escenarios y memoria (fase 7) | código legible y sin avisos al compilar, los 3 escenarios propios y la memoria con las cuestiones respondidas | 15 % |

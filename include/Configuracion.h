// Configuracion.h - TODOS los números que definen las reglas de la práctica, en un solo sitio.
//
// Cada aeronave guarda una copia de su configuración, así que puedes crear aeronaves
// "especiales":
//     ConfiguracionAvion c;
//     c.velocidadMaxima = 30;
//     Avion rapido("AV-99", {5, 5, 40}, Direccion::E, c);
//
// Unidades: distancias en casillas, tiempos en segundos, alturas en "unidades de altura".
#pragma once

// Parámetros comunes a todas las aeronaves.
struct ConfiguracionAeronave {
    double velocidadMinima = 0.0;       // casillas/s
    double velocidadMaxima = 10.0;
    double velocidadInicial = 5.0;      // velocidad al crearla
    double alturaMinima = 0.0;
    double alturaMaxima = 40.0;
    double velocidadAscenso = 5.0;      // unidades de altura por segundo
    double velocidadGiro = 0.0;         // grados/s. 0 = puede cambiar de rumbo al instante
    double combustibleMaximo = 1000.0;  // capacidad del depósito
    double consumoBase = 1.0;           // combustible/s solo por estar en vuelo
    double consumoPorVelocidad = 0.1;   // combustible/s adicional por cada casilla/s de velocidad
    double consumoEstacionario = 1.0;   // combustible/s en vuelo estacionario
    double toleranciaLlegadaXY = 1.0;   // distancia horizontal a un punto de ruta para darlo por alcanzado
    double toleranciaLlegadaZ = 1.5;    // diferencia de altura admitida al alcanzarlo
};

struct ConfiguracionAvion : ConfiguracionAeronave {
    ConfiguracionAvion() {
        velocidadMinima = 5.0;
        velocidadMaxima = 20.0;
        velocidadInicial = 10.0;
        alturaMinima = 10.0;
        alturaMaxima = 100.0;
        velocidadAscenso = 8.0;
        velocidadGiro = 120.0;  // grados/s
        combustibleMaximo = 1000.0;
        consumoBase = 1.0;
        consumoPorVelocidad = 0.15;
        consumoEstacionario = 0.0;  // no puede estar estacionario
        toleranciaLlegadaXY = 3.0;
        toleranciaLlegadaZ = 2.0;
    }
};

struct ConfiguracionHelicoptero : ConfiguracionAeronave {
    ConfiguracionHelicoptero() {
        velocidadMinima = 0.0;
        velocidadMaxima = 10.0;
        velocidadInicial = 5.0;
        alturaMinima = 0.0;
        alturaMaxima = 40.0;
        velocidadAscenso = 5.0;
        combustibleMaximo = 800.0;
        consumoBase = 1.5;
        consumoPorVelocidad = 0.2;
        consumoEstacionario = 2.5;  // estacionario cuesta MÁS que volar despacio
        toleranciaLlegadaXY = 1.0;
        toleranciaLlegadaZ = 1.5;
    }
};

struct ConfiguracionDron : ConfiguracionAeronave {
    double radioEspera = 3.0;  // radio de la zona de espera / vigilancia (casillas)

    ConfiguracionDron() {
        velocidadMinima = 0.0;
        velocidadMaxima = 8.0;
        velocidadInicial = 4.0;
        alturaMinima = 0.0;
        alturaMaxima = 20.0;
        velocidadAscenso = 12.0;  // cambia de altura deprisa
        combustibleMaximo = 300.0;
        consumoBase = 0.6;
        consumoPorVelocidad = 0.1;
        consumoEstacionario = 0.4;
        toleranciaLlegadaXY = 0.6;
        toleranciaLlegadaZ = 1.0;
    }
};

// Reglas del mundo (no de una aeronave concreta).
struct ConfiguracionSimulacion {
    // Colisión = casillas adyacentes en horizontal Y diferencia de altura <= umbral.
    double umbralColisionAltura = 2.0;
    // true: adyacentes son las 8 casillas que rodean (N, NE, E, SE, S, SW, W, NW) y la propia.
    // false: solo N, E, S, W y la propia.
    bool colisionEnDiagonales = true;
    double duracionExplosion = 5.0;   // segundos de simulación que se muestra la llama
    double pasoMaximo = 0.025;        // Tablero::avanzar(dt) subdivide dt en pasos no mayores que esto
    int alturaMaximaTablero = 120;    // techo del espacio aéreo
};

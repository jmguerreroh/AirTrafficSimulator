// Aeronave.h - Clase base ABSTRACTA de todas las aeronaves.
//
// ┌─ Ya implementado por el simulador ───────────────────────────────────────────────┐
// │ identidad, estado, ruta (añadir/insertar/eliminar/limpiar), combustible, colisiones │
// │ y utilidades de cálculo para escribir el movimiento (avanzar, girar, subir...).    │
// └────────────────────────────────────────────────────────────────────────────────────┘
// ┌─ A implementar en cada tipo (Avion, Helicoptero, Dron) ────────────────────────────┐
// │ actualizar(dt)  puedeMoverse(dir)  mover(dir)  calcularConsumo(dt)                  │
// └────────────────────────────────────────────────────────────────────────────────────┘
//
// Ciclo de un paso de simulación (lo hace Tablero::avanzar):
//   1. actualizar(dt)        ← tu código: mueve la aeronave
//   2. estado Volando / Estacionaria según se haya desplazado
//   3. consumo = calcularConsumo(dt) ← tu código; el simulador lo descuenta
//   4. ¿ha alcanzado el punto actual de su ruta? → siguiente punto (o Llegada)
//   5. se pinta la casilla visitada y se detectan colisiones
// Si en el paso 1 se sale del tablero, se estrella en el borde (FueraDeLimites) y no sigue.
#pragma once

#include <memory>
#include <string>
#include <vector>

#include "Color.h"
#include "Configuracion.h"
#include "Direccion.h"
#include "Posicion.h"
#include "PuntoRuta.h"
#include "Tipos.h"

class Aeronave {
public:
    virtual ~Aeronave() = default;

    // ------------------------------------------------------------------ identidad
    const std::string& getId() const { return id_; }  // "AV-01", "HE-02"... (vacío hasta añadirla al Tablero si no se dio)
    virtual TipoAeronave getTipo() const = 0;
    const Color& getColor() const { return color_; }
    bool tieneColor() const { return tieneColor_; }
    void setColor(const Color& color) { color_ = color; tieneColor_ = true; }  // debe ser único en el Tablero

    // ---------------------------------------------------------------------- estado
    Posicion getPosicion() const;  // celda (x, y) y altura redondeadas
    double getX() const { return x_; }  // posición exacta (continua)
    double getY() const { return y_; }
    double getAltura() const { return z_; }
    double getVelocidad() const { return velocidad_; }  // casillas/s
    double getCombustible() const { return combustible_; }
    double getCombustibleMaximo() const { return cfg_.combustibleMaximo; }
    double getPorcentajeCombustible() const { return 100.0 * combustible_ / cfg_.combustibleMaximo; }
    Direccion getRumbo() const { return direccionDesdeGrados(rumboGrados_); }
    double getRumboGrados() const { return rumboGrados_; }  // 0 = N, 90 = E
    EstadoAeronave getEstado() const { return estado_; }
    ModoControl getModoControl() const { return modo_; }
    bool estaActiva() const { return estado_ == EstadoAeronave::Volando || estado_ == EstadoAeronave::Estacionaria; }
    const ConfiguracionAeronave& getConfiguracion() const { return cfg_; }

    // ----------------------------------------------------------------------- ruta
    // Los puntos se visitan EN ORDEN. Todas las operaciones validan el punto
    // (dentro del tablero y dentro de los límites de altura de esta aeronave) y lanzan
    // std::invalid_argument / std::out_of_range si no es válido.
    const std::vector<PuntoRuta>& getRuta() const { return ruta_; }  // ruta completa (incluye los ya visitados)
    std::size_t getIndiceRuta() const { return indiceRuta_; }        // posición del punto actual
    bool tieneDestino() const { return indiceRuta_ < ruta_.size(); }
    const PuntoRuta& getDestinoActual() const;                       // std::logic_error si no hay destino
    void agregarPuntoRuta(const PuntoRuta& punto);                   // al final
    void insertarPuntoRuta(std::size_t indice, const PuntoRuta& p);  // en la posición 'indice' (0 = principio)
    void eliminarPuntoRuta(std::size_t indice);
    void limpiarRuta();
    void setIndiceRuta(std::size_t indice);                          // salta a otro punto de la ruta

    // ------------------------------------------------------------------- órdenes
    void setVelocidad(double velocidad);        // dentro de [velocidadMinima, velocidadMaxima]
    // Altura a la que debe ir, en cualquier modo. En modo Ruta tiene preferencia sobre la altura de los
    // puntos de la ruta: la aeronave sigue la ruta a esta altura (y da los puntos por alcanzados aunque
    // no esté a su altura) hasta que se llame a seguirRuta().
    void setAlturaObjetivo(double altura);
    void setCombustible(double combustible);    // (0, combustibleMaximo]
    // Devuelve el control al piloto automático: sigue la ruta, con la altura de sus puntos.
    void seguirRuta() { modo_ = ModoControl::Ruta; alturaOrdenada_ = false; }
    bool tieneAlturaOrdenada() const { return alturaOrdenada_; }  // ¿hay un setAlturaObjetivo vigente?
    // Altura hacia la que debe tender: la ordenada con setAlturaObjetivo o, en modo Ruta sin
    // altura ordenada, la del destino actual.
    double getAlturaObjetivo() const;

    // ------------------------------------- A IMPLEMENTAR EN CADA TIPO DE AERONAVE
    // Avanza la aeronave 'dt' segundos siguiendo su modo de control y sus reglas de movimiento.
    virtual void actualizar(double dt) = 0;
    // ¿Puede la aeronave tomar ahora esa dirección según sus restricciones?
    virtual bool puedeMoverse(Direccion direccion) const = 0;
    // Toma el control manual y ordena desplazarse en esa dirección (std::invalid_argument si !puedeMoverse).
    virtual void mover(Direccion direccion) = 0;
    // Combustible gastado durante 'dt' segundos en el estado actual.
    virtual double calcularConsumo(double dt) const = 0;
    // ------------------------------------------------------------------------------
    virtual std::unique_ptr<Aeronave> clonar() const = 0;  // copia polimórfica (la usa el Simulador)

    // Utilidades de geometría (públicas: útiles también para el ATC).
    double distanciaHorizontalA(const Posicion& p) const;
    double gradosHacia(const Posicion& p) const;  // rumbo desde aquí hasta p (0 = N, 90 = E)
    // Radio dentro del cual un punto de ruta se da por alcanzado (horizontalmente).
    // Para aeronaves con giro limitado crece con la velocidad (un avión rápido necesita más margen).
    double getRadioCaptura() const;

protected:
    Aeronave(const std::string& id, const Posicion& posicion, Direccion rumbo, const ConfiguracionAeronave& cfg);
    Aeronave(const Aeronave&) = default;
    Aeronave& operator=(const Aeronave&) = default;

    // ---- Herramientas para escribir actualizar() ------------------------------
    // Desplaza la posición 'distancia' casillas según un rumbo (grados) o una dirección.
    void avanzar(double rumboGrados, double distancia);
    void avanzar(Direccion direccion, double distancia);
    // Gira el rumbo hacia 'objetivoGrados' como máximo 'maxGrados' (por el lado más corto).
    void girarHaciaGrados(double objetivoGrados, double maxGrados);
    void fijarRumboGrados(double grados) { rumboGrados_ = normalizarGrados(grados); }
    // Acerca la altura a 'objetivo' cambiándola como máximo 'maxDelta' (respeta los límites del tipo).
    void acercarAltura(double objetivo, double maxDelta);
    void setModoControl(ModoControl modo) { modo_ = modo; }
    // Rumbo pedido con mover(): lo guarda mover() y lo consulta actualizar().
    double getRumboOrdenadoGrados() const { return rumboOrdenado_; }
    void setRumboOrdenadoGrados(double grados) { rumboOrdenado_ = normalizarGrados(grados); }
    // Lanza std::invalid_argument si el punto no es válido para esta aeronave.
    void validarPunto(const Posicion& p) const;

private:
    friend class Tablero;  // el Tablero es quien "hace avanzar el mundo"
    void asignarId(const std::string& id) { id_ = id; }
    void establecerLimitesTablero(int tamX, int tamY) { limiteX_ = tamX; limiteY_ = tamY; }
    void setEstado(EstadoAeronave e) { estado_ = e; }
    bool acotarAlTablero();          // la deja en el borde si se ha salido; true si se salió
    void consumirCombustible(double cantidad);
    bool comprobarLlegada();         // avanza el punto de ruta si se alcanzó; true si cambió algo
    void revisarFinDeRuta();

    std::string id_;
    Color color_;
    bool tieneColor_ = false;
    ConfiguracionAeronave cfg_;
    double x_, y_, z_;
    double velocidad_;
    double combustible_;
    double rumboGrados_;
    double rumboOrdenado_;
    double alturaObjetivo_;
    bool alturaOrdenada_ = false;  // setAlturaObjetivo vigente (hasta seguirRuta)
    EstadoAeronave estado_ = EstadoAeronave::Volando;
    ModoControl modo_ = ModoControl::Ruta;
    std::vector<PuntoRuta> ruta_;
    std::size_t indiceRuta_ = 0;
    int limiteX_ = 1 << 30;  // se fija al añadirla a un Tablero
    int limiteY_ = 1 << 30;
};

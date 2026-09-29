// Tipos.h - Enumeraciones comunes de las aeronaves.
#pragma once

enum class TipoAeronave { Avion, Helicoptero, Dron };

enum class EstadoAeronave {
    Volando,         // se ha desplazado horizontalmente en el último paso
    Estacionaria,    // no se ha desplazado horizontalmente (vuelo estacionario)
    SinCombustible,  // combustible agotado: queda inmóvil (sigue siendo un obstáculo)
    Llegada,         // completó su ruta: permanece visible, inmóvil, fuera del tráfico (no colisiona)
    Colisionada,     // ha chocado: queda inmóvil como restos (sigue siendo un obstáculo)
};

// Quién manda sobre la aeronave.
enum class ModoControl {
    Ruta,    // sigue su ruta automáticamente (piloto automático). Por defecto.
    Manual,  // obedece la última orden de mover(direccion) hasta que se llame a seguirRuta()
    Espera,  // (solo dron) permanece en una zona alrededor de un punto
};

inline const char* aTexto(TipoAeronave t) {
    switch (t) {
        case TipoAeronave::Avion: return "Avion";
        case TipoAeronave::Helicoptero: return "Helicoptero";
        case TipoAeronave::Dron: return "Dron";
    }
    return "?";
}

inline const char* aTexto(EstadoAeronave e) {
    switch (e) {
        case EstadoAeronave::Volando: return "Volando";
        case EstadoAeronave::Estacionaria: return "Estacionaria";
        case EstadoAeronave::SinCombustible: return "SinCombustible";
        case EstadoAeronave::Llegada: return "Llegada";
        case EstadoAeronave::Colisionada: return "Colisionada";
    }
    return "?";
}

inline const char* aTexto(ModoControl m) {
    switch (m) {
        case ModoControl::Ruta: return "Ruta";
        case ModoControl::Manual: return "Manual";
        case ModoControl::Espera: return "Espera";
    }
    return "?";
}

// Prefijo de los identificadores automáticos: AV-01, HE-01, DR-01...
inline const char* prefijoId(TipoAeronave t) {
    switch (t) {
        case TipoAeronave::Avion: return "AV";
        case TipoAeronave::Helicoptero: return "HE";
        case TipoAeronave::Dron: return "DR";
    }
    return "XX";
}

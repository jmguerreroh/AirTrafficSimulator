# PracticaATC.cmake - Funcion para el CMakeLists.txt del ALUMNO (cuando se trae este repositorio con FetchContent).
#
#   include(FetchContent)
#   FetchContent_Declare(ATC GIT_REPOSITORY https://github.com/jmguerreroh/AirTrafficSimulator.git GIT_TAG main)
#   FetchContent_MakeAvailable(ATC)
#   atc_practica()
#
# atc_practica():
#   1. La primera vez COPIA a la carpeta del alumno las plantillas que tiene que completar
#      (Avion, Helicoptero, Dron, ATC con sus .h) y un main.cpp de partida. Si ya existen, NO las toca.
#   2. Compila SOLO esos ficheros del alumno y los enlaza con el simulador (que compila CMake una vez).
#   3. Crea el ejecutable MiATC.
# Argumento opcional EJEMPLOS: tambien compila los programas de ejemplo del repositorio (prueba_*.cpp),
# usando las aeronaves DEL ALUMNO.
function(atc_practica)
  cmake_parse_arguments(A "EJEMPLOS" "NOMBRE" "" ${ARGN})
  if(NOT A_NOMBRE)
    set(A_NOMBRE MiATC)
  endif()
  get_filename_component(raiz "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/.." ABSOLUTE)
  set(dir "${CMAKE_SOURCE_DIR}")  # carpeta del proyecto del alumno

  foreach(f Avion.h Avion.cpp Helicoptero.h Helicoptero.cpp Dron.h Dron.cpp ATC.h ATC.cpp)
    if(NOT EXISTS "${dir}/${f}")
      file(COPY "${raiz}/students/${f}" DESTINATION "${dir}")
      message(STATUS "Plantilla creada en tu carpeta: ${f}")
    endif()
  endforeach()
  if(NOT EXISTS "${dir}/main.cpp")
    file(COPY "${raiz}/alumno/main.cpp" DESTINATION "${dir}")
    message(STATUS "Plantilla creada en tu carpeta: main.cpp")
  endif()

  # Tus ficheros: las aeronaves y el ATC forman una biblioteca; main.cpp, el ejecutable.
  add_library(aeronaves_alumno STATIC
    "${dir}/Avion.cpp" "${dir}/Helicoptero.cpp" "${dir}/Dron.cpp" "${dir}/ATC.cpp")
  target_include_directories(aeronaves_alumno PUBLIC "${dir}")
  target_link_libraries(aeronaves_alumno PUBLIC AirTrafficCoreBase)
  if(MSVC)
    target_compile_options(aeronaves_alumno PRIVATE /W4)
  else()
    target_compile_options(aeronaves_alumno PRIVATE -Wall -Wextra)
  endif()

  add_executable(${A_NOMBRE} "${dir}/main.cpp")
  target_link_libraries(${A_NOMBRE} PRIVATE aeronaves_alumno AirTrafficGraphics)

  if(A_EJEMPLOS)
    foreach(e prueba_aviones prueba_helicopteros prueba_drones prueba_colisiones
             prueba_varias prueba_rutas prueba_combustible ejemplo_atc)
      add_executable(${e} "${raiz}/examples/${e}.cpp")
      target_include_directories(${e} PRIVATE "${raiz}/examples")
      target_link_libraries(${e} PRIVATE aeronaves_alumno AirTrafficGraphics)
    endforeach()
  endif()
endfunction()

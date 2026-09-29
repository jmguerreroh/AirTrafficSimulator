cmake_minimum_required(VERSION 3.16)
project(AirTrafficSimulator VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Tipo de compilacion" FORCE)
endif()

# Solo se compilan ejemplos/tests cuando este es el proyecto principal.
if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
  set(ATC_IS_TOP_LEVEL ON)
else()
  set(ATC_IS_TOP_LEVEL OFF)
endif()
option(ATC_BUILD_EXAMPLES "Compilar los ejemplos" ${ATC_IS_TOP_LEVEL})
option(ATC_BUILD_TESTS "Compilar los tests" ${ATC_IS_TOP_LEVEL})

# ---- ¿Solucion del profesor o plantilla del alumno? ---------------------------------------------
# Si existe la carpeta solucion_profesor/, se compilan Avion/Helicoptero/Dron de referencia (los
# ejemplos funcionan "de fabrica"). Al repartir el proyecto a los alumnos se BORRA esa carpeta y CMake
# pasa solo a compilar students/ (plantilla con TODO). Se puede forzar con -DATC_USAR_SOLUCION_PROFESOR=OFF.
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/solucion_profesor/Avion.cpp)
  set(ATC_SOLUCION_POR_DEFECTO ON)
else()
  set(ATC_SOLUCION_POR_DEFECTO OFF)
endif()
option(ATC_USAR_SOLUCION_PROFESOR "Compilar la solucion de referencia en lugar de la plantilla" ${ATC_SOLUCION_POR_DEFECTO})
if(ATC_USAR_SOLUCION_PROFESOR)
  message(STATUS "Aeronaves: SOLUCION DEL PROFESOR (solucion_profesor/)")
  set(ATC_AERONAVES_SRC solucion_profesor/Avion.cpp solucion_profesor/Helicoptero.cpp solucion_profesor/Dron.cpp)
else()
  message(STATUS "Aeronaves: PLANTILLA DEL ALUMNO (students/)")
  set(ATC_AERONAVES_SRC students/src/Avion.cpp students/src/Helicoptero.cpp students/src/Dron.cpp)
endif()

include(${CMAKE_CURRENT_LIST_DIR}/cmake/PracticaATC.cmake)  # define atc_practica() para el CMakeLists del alumno

find_package(Threads REQUIRED)

# ---- GLFW: el del sistema si existe; si no, se descarga y compila ------------------------------
find_package(glfw3 3.3 QUIET)
if(NOT glfw3_FOUND)
  message(STATUS "GLFW no encontrado en el sistema: descargandolo con FetchContent")
  include(FetchContent)
  set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
  set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
  set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
  set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
  FetchContent_Declare(glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4)
  FetchContent_MakeAvailable(glfw)
endif()

# ---- GLM (solo cabeceras) y GLAD (OpenGL 3.3 Core), incluidos en third_party ----------------------
add_library(atc_glm INTERFACE)
target_include_directories(atc_glm SYSTEM INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/third_party/glm)

add_library(atc_glad STATIC third_party/glad/src/glad.c)
target_include_directories(atc_glad SYSTEM PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/third_party/glad/include)
target_link_libraries(atc_glad PUBLIC ${CMAKE_DL_LIBS})
set_target_properties(atc_glad PROPERTIES POSITION_INDEPENDENT_CODE ON)

# ---- Shaders embebidos: shaders/*.vert|frag -> cabecera generada --------------------------------
set(ATC_SHADER_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/shaders/basic.vert ${CMAKE_CURRENT_SOURCE_DIR}/shaders/basic.frag
  ${CMAKE_CURRENT_SOURCE_DIR}/shaders/hud.vert ${CMAKE_CURRENT_SOURCE_DIR}/shaders/hud.frag)
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${ATC_SHADER_FILES})
file(READ shaders/basic.vert BASIC_VERT_SOURCE)
file(READ shaders/basic.frag BASIC_FRAG_SOURCE)
file(READ shaders/hud.vert HUD_VERT_SOURCE)
file(READ shaders/hud.frag HUD_FRAG_SOURCE)
set(ATC_GENERATED_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated)
configure_file(cmake/shaders_embedded.h.in ${ATC_GENERATED_DIR}/shaders_embedded.h @ONLY)

function(atc_warnings target)
  if(MSVC)
    target_compile_options(${target} PRIVATE /W4)
  else()
    target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic)
  endif()
endfunction()

# ---- Nucleo logico (aeronaves base, tablero, colisiones): SIN OpenGL -------------------------------------
# Todo lo que NO modifica el alumno va en AirTrafficCoreBase + AirTrafficGraphics.
add_library(AirTrafficCoreBase STATIC
  src/Aeronave.cpp
  src/Tablero.cpp
  src/simulation/CollisionSystem.cpp)
target_include_directories(AirTrafficCoreBase
  PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/include
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
atc_warnings(AirTrafficCoreBase)

# ---- Ventana 3D + hilo de render ------------------------------------------------------------------------
add_library(AirTrafficGraphics STATIC
  src/Simulador.cpp
  src/RenderLoop.cpp
  src/window/Window.cpp
  src/rendering/AircraftModels.cpp
  src/rendering/BitmapFont.cpp
  src/rendering/Camera.cpp
  src/rendering/Mesh.cpp
  src/rendering/Renderer.cpp
  src/rendering/Shader.cpp)
target_include_directories(AirTrafficGraphics PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR}/src/rendering ${CMAKE_CURRENT_SOURCE_DIR}/src/window
  ${ATC_GENERATED_DIR})
target_link_libraries(AirTrafficGraphics PUBLIC AirTrafficCoreBase PRIVATE glfw atc_glad atc_glm Threads::Threads)
atc_warnings(AirTrafficGraphics)

# ---- Lo que escribe el alumno: Avion, Helicoptero, Dron y ATC (cabeceras en students/include, fuentes en students/src) ------------
add_library(AirTrafficAlumno STATIC ${ATC_AERONAVES_SRC} students/src/ATC.cpp)
target_include_directories(AirTrafficAlumno PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/students/include)
target_link_libraries(AirTrafficAlumno PUBLIC AirTrafficCoreBase)
atc_warnings(AirTrafficAlumno)

# Atajos: "todo el nucleo logico" (para tests, sin ventana) y "todo, con ventana" (para los programas).
add_library(AirTrafficCore INTERFACE)
target_link_libraries(AirTrafficCore INTERFACE AirTrafficAlumno AirTrafficCoreBase)
add_library(AirTrafficSimulator INTERFACE)
target_link_libraries(AirTrafficSimulator INTERFACE AirTrafficAlumno AirTrafficGraphics)
add_library(AirTrafficSimulator::AirTrafficSimulator ALIAS AirTrafficSimulator)

if(ATC_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()

# Programa del alumno: alumno/main.cpp -> ejecutable MiATC
if(ATC_IS_TOP_LEVEL AND EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/alumno/main.cpp)
  add_executable(MiATC alumno/main.cpp)
  target_link_libraries(MiATC PRIVATE AirTrafficSimulator)
endif()

if(ATC_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()

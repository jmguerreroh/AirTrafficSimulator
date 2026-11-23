# Tests del nucleo logico: no abren ventana (valen para CI sin pantalla).
# test_aeronaves (reglas de Avion/Helicoptero/Dron) y test_atc (el ATC evita colisiones en las escenas de
# examples/escenas.h) se ejecutan contra la solucion de referencia o, con -DATC_TESTS_ACEPTACION=ON, contra
# la plantilla del alumno (fallaran mientras queden TODO por completar).
option(ATC_TESTS_ACEPTACION "Compilar test_aeronaves y test_atc tambien contra la plantilla del alumno" OFF)
set(ATC_TESTS test_ruta test_colisiones)
if(ATC_USAR_SOLUCION_PROFESOR OR ATC_TESTS_ACEPTACION)
  list(APPEND ATC_TESTS test_aeronaves test_atc)
endif()

foreach(name ${ATC_TESTS})
  add_executable(${name} ${name}.cpp)
  target_link_libraries(${name} PRIVATE AirTrafficCore)
  target_include_directories(${name} PRIVATE ${PROJECT_SOURCE_DIR}/src ${PROJECT_SOURCE_DIR}/examples)
  add_test(NAME ${name} COMMAND ${name})
endforeach()

# Cámara y picking: usan cabeceras internas y GLM, pero no abren ninguna ventana.
add_executable(test_camera test_camera.cpp)
target_link_libraries(test_camera PRIVATE AirTrafficSimulator atc_glm)
target_include_directories(test_camera PRIVATE ${PROJECT_SOURCE_DIR}/src)
add_test(NAME test_camera COMMAND test_camera)

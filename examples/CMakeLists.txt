# Cada ejemplo es un ejecutable que solo enlaza con AirTrafficSimulator.
foreach(example
    prueba_aviones prueba_helicopteros prueba_drones prueba_colisiones
    prueba_varias prueba_rutas prueba_combustible ejemplo_atc)
  add_executable(${example} ${example}.cpp)
  target_link_libraries(${example} PRIVATE AirTrafficSimulator)
endforeach()

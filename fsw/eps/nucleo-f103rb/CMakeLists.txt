cmake_minimum_required(VERSION 3.20.0)

set(BOARD nucleo_f103rb)

find_package(Zephyr)
project(my_zephyr_app)

target_include_directories(app PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_sources(app PRIVATE
    src/stateMachine.c
    src/obcComm.c
    src/sensors.c
    src/main.c
)

cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

project(surakarta)
find_package(Torch REQUIRED)

add_executable(surakarta surakarta.cpp)

target_link_libraries(surakarta "${TORCH_LIBRARIES}")
set_property(TARGET surakarta PROPERTY CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-Wall -Ofast")

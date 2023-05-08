cmake_minimum_required(VERSION 3.25.0)

project(pechonk
  LANGUAGES CXX
)

cmake_path(REMOVE_FILENAME CMAKE_VS_DEVENV_COMMAND OUTPUT_VARIABLE _dia_path)
cmake_path(SET _dia_path NORMALIZE "${_dia_path}../../DIA SDK")

add_library(DIA INTERFACE)
target_include_directories(DIA
  INTERFACE
    "$<BUILD_INTERFACE:${_dia_path}/include>"
)
target_link_libraries(DIA
  INTERFACE
    "$<BUILD_INTERFACE:${_dia_path}/lib/amd64/diaguids.lib>"
)

add_library(WIL INTERFACE)
target_include_directories(WIL
  INTERFACE
    "$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/third_party/wil/include>"
)

add_executable(pechonk
  "src/main.cpp"
)
target_link_libraries(pechonk
  WIL
  DIA
)

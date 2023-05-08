cmake_minimum_required(VERSION 3.25.0)

project(pechonk
  LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<$<CONFIG:Debug,RelWithDebInfo,Release>:ProgramDatabase>")
add_compile_options(/W4 /WX)

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
  "src/dia_helpers.h"
  "src/dia_helpers.cpp"
  "src/main.cpp"
)
target_link_libraries(pechonk
  WIL
  DIA
)

set(_dia_dll "${_dia_path}/bin/amd64/msdia140.dll")
add_custom_command(TARGET pechonk POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${_dia_dll} "$<TARGET_FILE_DIR:pechonk>/"
)

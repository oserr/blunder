cmake_minimum_required(VERSION 3.23)
project(blunder CXX)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (NOT CMAKE_BUILD_TYPE OR (CMAKE_BUILD_TYPE STREQUAL ""))
  set(CMAKE_BUILD_TYPE "Debug")
endif()

string(TOLOWER "${CMAKE_BUILD_TYPE}" build_type)
message(STATUS "Building ${CMAKE_PROJECT_NAME} in ${build_type} mode")

if (build_type STREQUAL "debug")
  add_compile_options(-Wall -Wextra -Wpedantic -Og)
else()
  add_compile_definitions(NDEBUG)
  add_compile_options(-Wall -Wextra -Wpedantic -Ofast)
endif()

add_library(blunder STATIC
  src/bitboard.cc
  src/bitboard.h
  src/bitboard_io.cc
  src/bitboard_io.h
  src/board_side.h
  src/board_state.h
  src/color.h
  src/error.h
  src/magics.cc
  src/magics.h
  src/move.h
  src/move_gen.cc
  src/move_gen.h
  src/move_generator.h
  src/moves.h
  src/pieces.h
  src/pre_computed_magics.h)
target_compile_features(blunder PRIVATE)

add_executable(bbprinter src/bbprinter.cc)
target_compile_features(bbprinter PRIVATE)
target_link_libraries(bbprinter blunder)

add_executable(genmagic src/genmagic.cc)
target_compile_features(genmagic PRIVATE)
target_link_libraries(genmagic blunder)

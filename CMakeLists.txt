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

include(FetchContent)

# Googletest
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.13.0)
FetchContent_MakeAvailable(googletest)

add_library(blunder STATIC
  src/bitboard.cc
  src/bitboard.h
  src/bitboard_io.cc
  src/bitboard_io.h
  src/board_side.h
  src/board_state.h
  src/color.h
  src/err.h
  src/magics.cc
  src/magics.h
  src/move.h
  src/move_gen.cc
  src/move_gen.h
  src/move_generator.h
  src/moves.h
  src/pieces.h
  src/pre_computed_magics.h
  src/square.cc
  src/square.h)
target_compile_features(blunder PRIVATE)

add_executable(bbprinter src/bbprinter.cc)
target_compile_features(bbprinter PRIVATE)
target_link_libraries(bbprinter blunder)

add_executable(genmagic src/genmagic.cc)
target_compile_features(genmagic PRIVATE)
target_link_libraries(genmagic blunder)

include(GoogleTest)

# TODO: use for loop to build unit tests.

add_executable(moves_test test/moves_test.cc)
target_include_directories(moves_test PUBLIC src)
target_compile_features(moves_test PRIVATE)
target_link_libraries(moves_test blunder GTest::gmock_main)

add_executable(square_test test/square_test.cc)
target_include_directories(square_test PUBLIC src)
target_compile_features(square_test PRIVATE)
target_link_libraries(square_test blunder GTest::gmock_main)

add_executable(magics_test test/magics_test.cc)
target_include_directories(magics_test PUBLIC src)
target_compile_features(magics_test PRIVATE)
target_link_libraries(magics_test blunder GTest::gmock_main)

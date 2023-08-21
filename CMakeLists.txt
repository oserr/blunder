cmake_minimum_required(VERSION 3.23)
project(blunder CXX)
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

# The torch library defines the following 3 variables used below:
# - TORCH_CXX_FLAGS
# - TORCH_INCLUDE_DIRS
# - TORCH_LIBRARIES
if (DEFINED ENV{TORCH_INSTALL_PREFIX})
  # If TORCH_INSTALL_PREFIX is defined, then we'll prioritize building with the
  # torch lib installed there, and hence HINTS tell find_package to look in the
  # TORCH_INSTALL_PREFIX before looking in system directories.
  find_package(Torch REQUIRED HINTS $ENV{TORCH_INSTALL_PREFIX})
else()
  find_package(Torch REQUIRED)
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
  src/board.cc
  src/board.h
  src/board_path.h
  src/board_side.h
  src/color.h
  src/err.h
  src/evaluator.h
  src/fen.cc
  src/fen.h
  src/game.h
  src/game_state.h
  src/hash.h
  src/magic_attacks.cc
  src/magic_attacks.h
  src/magics.h
  src/mcts.cc
  src/mcts.h
  src/move.cc
  src/move.h
  src/moves.h
  src/net.cc
  src/net.h
  src/pieces.cc
  src/pieces.h
  src/piece_set.cc
  src/piece_set.h
  src/player.h
  src/pre_computed_magics.h
  src/random_player.h
  src/random_search.cc
  src/random_search.h
  src/search.h
  src/simple_game.cc
  src/simple_game.h
  src/square.cc
  src/square.h
  src/square_iter.h
  src/terminal_player.h
  src/terminal_player.cc)
set_target_properties(blunder PROPERTIES CXX_STANDARD 23)
target_compile_options(blunder PUBLIC ${TORCH_CXX_FLAGS})
target_include_directories(blunder PUBLIC ${TORCH_INCLUDE_DIRS})
target_compile_features(blunder PRIVATE)
target_link_libraries(blunder ${TORCH_LIBRARIES})

add_executable(bbprinter src/bbprinter.cc)
set_target_properties(bbprinter PROPERTIES CXX_STANDARD 23)
target_compile_features(bbprinter PRIVATE)
target_link_libraries(bbprinter blunder)

add_executable(genmagic src/genmagic.cc)
set_target_properties(genmagic PROPERTIES CXX_STANDARD 23)
target_compile_features(genmagic PRIVATE)
target_link_libraries(genmagic blunder)

add_executable(train_net src/train_net.cc)
set_target_properties(train_net PROPERTIES CXX_STANDARD 23)
target_compile_features(train_net PRIVATE)
target_link_libraries(train_net blunder)

add_executable(game src/terminal_game.cc)
set_target_properties(game PROPERTIES CXX_STANDARD 23)
target_compile_features(game PRIVATE)
target_link_libraries(game blunder)

# Simple function to create a test target.
function(create_test target)
  add_executable(${target}_test test/${target}_test.cc)
  set_target_properties(${target}_test PROPERTIES CXX_STANDARD 23)
  target_include_directories(${target}_test PUBLIC src)
  target_compile_features(${target}_test PRIVATE)
  target_link_libraries(${target}_test blunder GTest::gmock_main)
endfunction()

create_test(moves)
create_test(square)
create_test(magic_attacks)
create_test(bitboard)
create_test(fen)
create_test(board)
create_test(board_path)

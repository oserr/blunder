cmake_minimum_required(VERSION 3.23)
project(blunder CXX)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (NOT CMAKE_BUILD_TYPE OR (CMAKE_BUILD_TYPE STREQUAL ""))
  set(CMAKE_BUILD_TYPE "Debug")
endif()

string(TOLOWER "${CMAKE_BUILD_TYPE}" build_type)
message(STATUS "Building ${CMAKE_PROJECT_NAME} in ${build_type} mode")

set(CAFFE2_USE_CUDNN ON)

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

# Par
FetchContent_Declare(
  par
  GIT_REPOSITORY https://github.com/oserr/par.git
  GIT_TAG 31e8018a7311cc8ff09b8cbbd5a786a6fb216e3e)
FetchContent_MakeAvailable(par)

add_library(blunder STATIC
  src/alpha_zero_decoder.cc
  src/alpha_zero_decoder.h
  src/alpha_zero_encoder.cc
  src/alpha_zero_encoder.h
  src/alpha_zero_evaluator.cc
  src/alpha_zero_evaluator.h
  src/bitboard.cc
  src/bitboard.h
  src/blunder_player.cc
  src/blunder_player.h
  src/board.cc
  src/board.h
  src/board_path.h
  src/board_side.h
  src/chess_data_set.cc
  src/chess_data_set.h
  src/coding_util.cc
  src/coding_util.h
  src/color.h
  src/err.h
  src/evaluator.h
  src/fen.cc
  src/fen.h
  src/game.h
  src/game_result.cc
  src/game_result.h
  src/game_state.h
  src/game_winner.cc
  src/game_winner.h
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
  src/search_result.h
  src/simple_game.cc
  src/simple_game.h
  src/simple_game_builder.cc
  src/simple_game_builder.h
  src/square.cc
  src/square.h
  src/square_iter.h
  src/tensor_decoder.h
  src/tensor_encoder.h
  src/timer.cc
  src/timer.h
  src/time_types.h
  src/terminal_player.h
  src/terminal_player.cc
  src/trainer.h
  src/trainer.cc
  src/trainer_builder.cc
  src/trainer_builder.h)
set_target_properties(blunder PROPERTIES CXX_STANDARD 23)
target_compile_options(blunder PUBLIC ${TORCH_CXX_FLAGS})
target_include_directories(blunder PUBLIC
  ${TORCH_INCLUDE_DIRS}
  ${par_INCLUDE_DIRS})
target_compile_features(blunder PRIVATE)
target_link_libraries(blunder ${TORCH_LIBRARIES} par)

# Simple function to create a test target.
function(create_target target)
  add_executable(${target} src/${target}.cc)
  set_target_properties(${target} PROPERTIES CXX_STANDARD 23)
  target_compile_features(${target} PRIVATE)
  target_link_libraries(${target} blunder)
endfunction()

create_target(bbprinter)
create_target(genmagic)
create_target(train_net)
create_target(terminal_game)
create_target(training)

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

# Simple function to create a bench target.
function(create_bench target)
  add_executable(${target}_bench benches/${target}.cc)
  set_target_properties(${target}_bench PROPERTIES CXX_STANDARD 23)
  target_include_directories(${target}_bench PUBLIC src)
  target_compile_features(${target}_bench PRIVATE)
  target_link_libraries(${target}_bench blunder)
endfunction()

create_bench(magics)

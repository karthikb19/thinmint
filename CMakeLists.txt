cmake_minimum_required(VERSION 3.20)

project(thinmint VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(CTest)

add_library(thinmint_core
  src/core/version.cpp
  src/core/square.cpp
  src/core/piece.cpp
  src/core/bitboard.cpp
  src/core/move.cpp
  src/board/board.cpp
  src/board/makemove.cpp
  src/board/unmakemove.cpp
  src/movegen/attacks.cpp
  src/movegen/movegen.cpp
  src/perft/perft.cpp
  src/uci/protocol.cpp
)

target_include_directories(thinmint_core
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(thinmint_core PUBLIC cxx_std_20)

if(MSVC)
  target_compile_options(thinmint_core PRIVATE /W4 /permissive-)
else()
  target_compile_options(thinmint_core PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_executable(thinmint
  src/main.cpp
)

target_link_libraries(thinmint PRIVATE thinmint_core)

if(BUILD_TESTING)
  add_executable(thinmint_smoke_test
    tests/unit/smoke_test.cpp
  )

  target_link_libraries(thinmint_smoke_test PRIVATE thinmint_core)

  add_test(NAME thinmint_smoke_test COMMAND thinmint_smoke_test)

  add_executable(thinmint_square_test
    tests/unit/square_test.cpp
  )

  target_link_libraries(thinmint_square_test PRIVATE thinmint_core)

  add_test(NAME thinmint_square_test COMMAND thinmint_square_test)

  add_executable(thinmint_piece_test
    tests/unit/piece_test.cpp
  )

  target_link_libraries(thinmint_piece_test PRIVATE thinmint_core)

  add_test(NAME thinmint_piece_test COMMAND thinmint_piece_test)

  add_executable(thinmint_bitboard_test
    tests/unit/bitboard_test.cpp
  )

  target_link_libraries(thinmint_bitboard_test PRIVATE thinmint_core)

  add_test(NAME thinmint_bitboard_test COMMAND thinmint_bitboard_test)

  add_executable(thinmint_move_test
    tests/unit/move_test.cpp
  )

  target_link_libraries(thinmint_move_test PRIVATE thinmint_core)

  add_test(NAME thinmint_move_test COMMAND thinmint_move_test)

  add_executable(thinmint_board_test
    tests/unit/board_test.cpp
  )

  target_link_libraries(thinmint_board_test PRIVATE thinmint_core)

  add_test(NAME thinmint_board_test COMMAND thinmint_board_test)

  add_executable(thinmint_fen_test
    tests/unit/fen_test.cpp
  )

  target_link_libraries(thinmint_fen_test PRIVATE thinmint_core)

  add_test(NAME thinmint_fen_test COMMAND thinmint_fen_test)

  add_executable(thinmint_pawn_attacks_test
    tests/unit/pawn_attacks_test.cpp
  )

  target_link_libraries(thinmint_pawn_attacks_test PRIVATE thinmint_core)

  add_test(NAME thinmint_pawn_attacks_test COMMAND thinmint_pawn_attacks_test)

  add_executable(thinmint_knight_king_attacks_test
    tests/unit/knight_king_attacks_test.cpp
  )

  target_link_libraries(thinmint_knight_king_attacks_test PRIVATE thinmint_core)

  add_test(NAME thinmint_knight_king_attacks_test COMMAND thinmint_knight_king_attacks_test)

  add_executable(thinmint_sliding_attacks_test
    tests/unit/sliding_attacks_test.cpp
  )

  target_link_libraries(thinmint_sliding_attacks_test PRIVATE thinmint_core)

  add_test(NAME thinmint_sliding_attacks_test COMMAND thinmint_sliding_attacks_test)

  add_executable(thinmint_pseudolegal_movegen_test
    tests/unit/pseudolegal_movegen_test.cpp
  )

  target_link_libraries(thinmint_pseudolegal_movegen_test PRIVATE thinmint_core)

  add_test(NAME thinmint_pseudolegal_movegen_test COMMAND thinmint_pseudolegal_movegen_test)

  add_executable(thinmint_check_detection_test
    tests/unit/check_detection_test.cpp
  )

  target_link_libraries(thinmint_check_detection_test PRIVATE thinmint_core)

  add_test(NAME thinmint_check_detection_test COMMAND thinmint_check_detection_test)

  add_executable(thinmint_castling_ep_test
    tests/unit/castling_ep_test.cpp
  )

  target_link_libraries(thinmint_castling_ep_test PRIVATE thinmint_core)

  add_test(NAME thinmint_castling_ep_test COMMAND thinmint_castling_ep_test)

  add_executable(thinmint_legal_movegen_test
    tests/unit/legal_movegen_test.cpp
  )

  target_link_libraries(thinmint_legal_movegen_test PRIVATE thinmint_core)

  add_test(NAME thinmint_legal_movegen_test COMMAND thinmint_legal_movegen_test)

  add_executable(thinmint_perft_test
    tests/unit/perft_test.cpp
  )

  target_link_libraries(thinmint_perft_test PRIVATE thinmint_core)

  add_test(NAME thinmint_perft_test COMMAND thinmint_perft_test)

  add_executable(thinmint_makemove_test
    tests/unit/makemove_test.cpp
  )

  target_link_libraries(thinmint_makemove_test PRIVATE thinmint_core)

  add_test(NAME thinmint_makemove_test COMMAND thinmint_makemove_test)

  add_executable(thinmint_unmakemove_test
    tests/unit/unmakemove_test.cpp
  )

  target_link_libraries(thinmint_unmakemove_test PRIVATE thinmint_core)

  add_test(NAME thinmint_unmakemove_test COMMAND thinmint_unmakemove_test)

  add_executable(thinmint_make_unmake_stress_test
    tests/unit/make_unmake_stress_test.cpp
  )

  target_link_libraries(thinmint_make_unmake_stress_test PRIVATE thinmint_core)

  add_test(NAME thinmint_make_unmake_stress_test COMMAND thinmint_make_unmake_stress_test)

  add_executable(thinmint_uci_protocol_test
    tests/unit/uci_protocol_test.cpp
  )

  target_link_libraries(thinmint_uci_protocol_test PRIVATE thinmint_core)

  add_test(NAME thinmint_uci_protocol_test COMMAND thinmint_uci_protocol_test)

  add_executable(thinmint_uci_position_test
    tests/unit/uci_position_test.cpp
  )

  target_link_libraries(thinmint_uci_position_test PRIVATE thinmint_core)

  add_test(NAME thinmint_uci_position_test COMMAND thinmint_uci_position_test)
endif()

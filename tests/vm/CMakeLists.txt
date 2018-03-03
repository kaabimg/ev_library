
add_executable (parser_tst parser.cpp)
target_link_libraries(parser_tst ev_vm LLVM-3.9 rt dl pthread z m pthread)

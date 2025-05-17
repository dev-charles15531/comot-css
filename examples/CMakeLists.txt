add_executable(main main.c)
target_link_libraries(main PRIVATE comot-css arena_alloc)

# Apply Sanitizers (only to the final executable)
if(TOK_ENABLE_SANITIZERS AND CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
  target_compile_options(main PRIVATE -fsanitize=address,undefined)
  target_link_options(main PRIVATE -fsanitize=address,undefined)
endif()

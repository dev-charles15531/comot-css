#---------------------------------------------------------------------
# comot-css Core Library (static)
#---------------------------------------------------------------------
add_library(comot-css STATIC)

target_link_libraries(comot-css PRIVATE arena_alloc)

# Subcomponents
target_sources(comot-css PRIVATE
  tokenizer/consume_comment_or_delim.c
  tokenizer/consume_escaped_code_point.c
  tokenizer/consume_ident_like_token.c
  tokenizer/consume_ident_sequence.c
  tokenizer/consume_numeric_token.c
  tokenizer/consume_string.c
  tokenizer/consume_url_token.c
  tokenizer/reconsume_curr_code_point.c
  tokenizer/tokenizer.c
  tokenizer/tokenizer_impl.c

  utils/decoder.c
  utils/diag.c
)

# Private headers
target_include_directories(comot-css PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/tokenizer/priv
  ${CMAKE_CURRENT_SOURCE_DIR}/utils
)

# Public API headers (installed with library)
target_include_directories(comot-css PUBLIC
  $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

#---------------------------------------------------------------------
# Compiler Warnings & Sanitizers
#---------------------------------------------------------------------
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
  # Common warning flags
  target_compile_options(comot-css PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    -Wformat=2
    -Wstrict-aliasing=2
    -Wmissing-prototypes
    -Wnull-dereference
    -Wstack-protector
  )

  # Treat warnings as errors (optional)
  if(TOK_WARNINGS_AS_ERRORS)
    target_compile_options(comot-css PRIVATE -Werror)
  endif()
elseif(MSVC)
  add_compile_options(/W4 /WX)
endif()

#---------------------------------------------------------------------
# Installation Rules (for package managers)
#---------------------------------------------------------------------
install(TARGETS comot-css
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/tokenizer
  DESTINATION include
)

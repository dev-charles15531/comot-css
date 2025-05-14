#---------------------------------------------------------------------
# Lexer Core Library (static)
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

#---------------------------------------------------------------------
# Lexer Core Library (static)
#---------------------------------------------------------------------
add_library(comot-css STATIC)

target_link_libraries(comot-css PRIVATE arena_alloc)

# Subcomponents
target_sources(comot-css PRIVATE
    tokenizer/tokenizer.c
    tokenizer/tokenizer_impl.c
    tokenizer/consume_comment_or_delim.c

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

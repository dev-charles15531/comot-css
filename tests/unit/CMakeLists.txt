add_executable(css_tokenizer_unit_test cssTokenizerUnitTests.c)

# Add the tokenizer library (from src/)
target_link_libraries(css_tokenizer_unit_test PRIVATE comot-css)

# Include project headers
target_include_directories(css_tokenizer_unit_test PRIVATE ${PROJECT_SOURCE_DIR}/include)

# Enable sanitizers
target_compile_options(css_tokenizer_unit_test PRIVATE -fsanitize=address,undefined)
target_link_options(css_tokenizer_unit_test PRIVATE -fsanitize=address,undefined)

add_executable(css_tokenizer_fuzz cssTokenizerFuzz.c)

# Add the tokenizer library (from src/)
target_link_libraries(css_tokenizer_fuzz PRIVATE comot-css)

# Include project headers
target_include_directories(css_tokenizer_fuzz PRIVATE ${PROJECT_SOURCE_DIR}/include)

# Enable sanitizers and libFuzzer
target_compile_options(css_tokenizer_fuzz PRIVATE -fsanitize=fuzzer,address,undefined)
target_link_options(css_tokenizer_fuzz PRIVATE -fsanitize=fuzzer,address,undefined)

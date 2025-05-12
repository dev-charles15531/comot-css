cmake_minimum_required(VERSION 3.15)
project(comot-css LANGUAGES C)

include(FetchContent)

#---------------------------------------------------------------------
# Fetch External Lib
#---------------------------------------------------------------------
FetchContent_Declare(
    arena_alloc
    GIT_REPOSITORY https://github.com/dev-charles15531/arena-allocator.git
    GIT_TAG        main 
)

FetchContent_MakeAvailable(arena_alloc)
# Add arena_alloc headers globally
include_directories(${arena_alloc_SOURCE_DIR}/include)

#---------------------------------------------------------------------
# Safety & Compiler Flags
#---------------------------------------------------------------------
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)  # Disable compiler-specific extensions

if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    # Warnings (paranoid mode)
    add_compile_options(
        -Wall
        -Wextra
        -Wpedantic
        -Wformat=2
        -Wstrict-aliasing=2
        -Wmissing-prototypes
        -Wnull-dereference
        -Wstack-protector
    )
    
    # Treat warnings as errors in CI
    if(LEXER_WARNINGS_AS_ERRORS)
        add_compile_options(-Werror)
    endif()
    
    # Sanitizers (optional)
    option(LEXER_ENABLE_SANITIZERS "Enable Address/Undefined sanitizers" ON)
    if(LEXER_ENABLE_SANITIZERS)
        add_compile_options(-fsanitize=address,undefined)
        add_link_options(-fsanitize=address,undefined)
    endif()
elseif(MSVC)
    add_compile_options(/W4 /WX)
endif()

#---------------------------------------------------------------------
# Project Structure
#---------------------------------------------------------------------
# Public headers (exposed to users)
include_directories(include)

# Core library
add_subdirectory(src)

# Tests (only build if requested)
option(BUILD_TESTS "Build lexer tests" OFF)
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Examples (optional)
option(BUILD_EXAMPLES "Build example programs" ON)
if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

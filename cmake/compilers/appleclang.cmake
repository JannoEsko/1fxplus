# Apple Clang compiler specific settings

if(NOT CMAKE_C_COMPILER_ID STREQUAL "AppleClang")
    return()
endif()

add_compile_options(-Wno-int-conversion)

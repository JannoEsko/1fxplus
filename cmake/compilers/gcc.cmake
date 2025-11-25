# GCC compiler specific settings

if(NOT CMAKE_C_COMPILER_ID STREQUAL "GNU")
    return()
endif()

add_compile_options(-Wno-int-conversion)
add_compile_options(-Wno-incompatible-pointer-types)

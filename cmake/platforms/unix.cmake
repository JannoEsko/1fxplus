# Unix specific settings (this includes macOS and emscripten)

if(NOT UNIX)
    return()
endif()

# For more context on pthread and cURL, refer to code/game/1fx/threads.c

list(APPEND COMMON_LIBRARIES
    dl  # Dynamic loader
    m   # Math library
    pthread 
)

if(FORCE_32BIT)
# Force it to find x86 libs
    set(CMAKE_SYSTEM_PROCESSOR x86)
    set(CMAKE_LIBRARY_ARCHITECTURE i386-linux-gnu)
    
    set(CMAKE_LIBRARY_PATH /usr/lib/i386-linux-gnu)
    set(CMAKE_INCLUDE_PATH /usr/include/i386-linux-gnu)
endif()

find_package(CURL REQUIRED)
list(APPEND COMMON_LIBRARIES CURL::libcurl)

find_package(ZLIB REQUIRED)
list(APPEND COMMON_LIBRARIES ZLIB::ZLIB)

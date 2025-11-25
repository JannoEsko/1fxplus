# Windows specific settings

if(NOT WIN32)
    return()
endif()

list(APPEND COMMON_LIBRARIES
    user32
    advapi32
    winmm
    wsock32
    ws2_32
    crypt32
    iphlpapi
    secur32
)

if(MINGW)
    list(APPEND COMMON_LIBRARIES
        mingw32
    )
endif()

find_package(CURL REQUIRED)
list(APPEND COMMON_LIBRARIES CURL::libcurl)

find_package(ZLIB REQUIRED)
list(APPEND COMMON_LIBRARIES ZLIB::ZLIB)


cmake_minimum_required(VERSION 3.18)

if(POLICY CMP0177)
    # Policy CMP0177: install() DESTINATION paths are normalized
    # Introduced in CMake 3.31
    cmake_policy(SET CMP0177 NEW)
endif()

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
include(identity)

project(${PROJECT_NAME} VERSION ${PROJECT_VERSION} LANGUAGES C)

option(BUILD_GAMETYPES "Build the gametype libraries" ON)
option(BUILD_GAME "Build the game library" ON)

option(BUILD_GT_HNS "Build H&S gametype" ON)
option(BUILD_GT_DEM "Build DEM gametype" OFF) # This is kept off just because in a multiprotocol state, some DEM gametype modules just don't work *correctly*
option(BUILD_GT_INF "Build INF gametype" ON)
option(BUILD_GT_CTF "Build CTF gametype" ON)
option(BUILD_GT_DM  "Build DM gametype" ON)
option(BUILD_GT_TDM "Build TDM gametype" ON)
option(BUILD_GT_ELIM "Build Elimination gametype" ON)
option(BUILD_GT_HNZ "Build H&Z gametype" ON)
option(BUILD_GT_CSINF "Build CSINF gametype" ON)
option(BUILD_GT_VIP "Build Kill the VIP gametype" ON)
option(BUILD_GT_PROP "Build Prophunt gametype" ON)
option(BUILD_GT_GUNGAME "Build Gungame gametype" ON)
option(SOF2_BOTS "Include SoF2 bots" ON)


# Release build by default, set externally if you want something else
if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_SHARED_LIBRARY_PREFIX "")

set(CMAKE_USE_RELATIVE_PATHS TRUE)

if(NOT PRODUCT_VERSION)
    set(PRODUCT_VERSION "${CMAKE_PROJECT_VERSION}")

    if(EXISTS "${CMAKE_SOURCE_DIR}/.git")
        execute_process(
            COMMAND git show -s --pretty=format:%h
            WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
            OUTPUT_VARIABLE GIT_REV
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET)

        if(GIT_REV)
            set(PRODUCT_VERSION "${PRODUCT_VERSION}_g${GIT_REV}")
        endif()
    endif()
endif()

add_compile_definitions(PRODUCT_VERSION="${PRODUCT_VERSION}")
add_compile_definitions(
        $<$<CONFIG:Debug>:_DEBUG>
        $<$<CONFIG:Release>:NDEBUG>
)

if(SOF2_BOTS)
        add_compile_definitions(_SOF2_BOTS)
endif()

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR})

# For CI to read from
file(GENERATE OUTPUT ${CMAKE_BINARY_DIR}/version.txt CONTENT ${PRODUCT_VERSION})

if(DEFINED ENV{SOURCE_DATE_EPOCH})
    string(TIMESTAMP PRODUCT_DATE "%b %d %Y" UTC)
    add_compile_definitions(PRODUCT_DATE="${PRODUCT_DATE}")
endif()

set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/code)

if(FORCE_32BIT)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -m32")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -m32")
    set(ARCH "x86")
endif()

include(utils/arch)
include(compilers/all)
include(platforms/all)

include(game)
include(gametype)

include(post_configure)

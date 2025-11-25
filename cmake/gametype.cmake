
if(NOT BUILD_GAMETYPES)
    return()
endif()

set(GT_SHARED_SOURCES
    ${SOURCE_DIR}/gametype/gt_shared.c
    ${SOURCE_DIR}/gametype/gt_syscalls.c
    ${SOURCE_DIR}/qcommon/q_shared.c
)

include(gametypes/all)
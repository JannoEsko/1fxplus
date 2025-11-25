
if(NOT BUILD_GT_CSINF OR NOT BUILD_GAMETYPES)
    return()
endif()

set(GT_NAME "csinf")

set(GT_SOURCES 
    ${SOURCE_DIR}/gametype/gt_${GT_NAME}/gt_main.c
    ${SOURCE_DIR}/qcommon/q_math.c # I use Com_Clamp in csinf. Probably should instead just add a function for it...
    ${GT_SHARED_SOURCES}
)

add_library(gt_${GT_NAME} SHARED ${GT_SOURCES})

set_target_properties(gt_${GT_NAME} PROPERTIES
    OUTPUT_NAME "gt_${GT_NAME}${ARCH}"
    PREFIX ""
    ARCHIVE_OUTPUT_NAME ""
)


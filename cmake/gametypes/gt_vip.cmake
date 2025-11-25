
if(NOT BUILD_GT_VIP OR NOT BUILD_GAMETYPES)
    return()
endif()

set(GT_NAME "vip")

set(GT_SOURCES 
    ${SOURCE_DIR}/gametype/gt_${GT_NAME}/gt_main.c
    ${GT_SHARED_SOURCES}
)

add_library(gt_${GT_NAME} SHARED ${GT_SOURCES})

set_target_properties(gt_${GT_NAME} PROPERTIES
    OUTPUT_NAME "gt_${GT_NAME}${ARCH}"
    PREFIX ""
    ARCHIVE_OUTPUT_NAME ""
)


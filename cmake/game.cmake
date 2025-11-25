
if(NOT BUILD_GAME)
    return()
endif()

#include(utils/set_output_dirs)

set(GAME_LIB "sof2mp_game")

set(GAME_SOURCES 
    ${SOURCE_DIR}/game/g_main.c
    ${SOURCE_DIR}/game/ai_main.c
    ${SOURCE_DIR}/game/ai_util.c
    ${SOURCE_DIR}/game/ai_wpnav.c
    ${SOURCE_DIR}/game/bg_gametype.c
    ${SOURCE_DIR}/game/bg_misc.c
    ${SOURCE_DIR}/game/bg_player.c
    ${SOURCE_DIR}/game/bg_pmove.c
    ${SOURCE_DIR}/game/bg_slidemove.c
    ${SOURCE_DIR}/game/bg_weapons.c
    ${SOURCE_DIR}/game/g_active.c
    ${SOURCE_DIR}/game/g_antilag.c
    ${SOURCE_DIR}/game/g_bot.c
    ${SOURCE_DIR}/game/g_client.c
    ${SOURCE_DIR}/game/g_cmds.c
    ${SOURCE_DIR}/game/g_combat.c
    ${SOURCE_DIR}/game/g_gametype.c
    ${SOURCE_DIR}/game/g_items.c
    ${SOURCE_DIR}/game/g_mem.c
    ${SOURCE_DIR}/game/g_misc.c
    ${SOURCE_DIR}/game/g_missile.c
    ${SOURCE_DIR}/game/g_mover.c
    ${SOURCE_DIR}/game/g_session.c
    ${SOURCE_DIR}/game/g_spawn.c
    ${SOURCE_DIR}/game/g_svcmds.c
    ${SOURCE_DIR}/game/g_syscalls.c
    ${SOURCE_DIR}/game/g_target.c
    ${SOURCE_DIR}/game/g_team.c
    ${SOURCE_DIR}/game/g_trigger.c
    ${SOURCE_DIR}/game/g_utils.c
    ${SOURCE_DIR}/game/g_weapon.c
    
    ${SOURCE_DIR}/ext/sqlite/sqlite3.c
    
    ${SOURCE_DIR}/qcommon/q_shared.c
    ${SOURCE_DIR}/qcommon/q_math.c
    
    ${SOURCE_DIR}/game/1fx/admin.c
    ${SOURCE_DIR}/game/1fx/databases.c
    ${SOURCE_DIR}/game/1fx/functions.c
    ${SOURCE_DIR}/game/1fx/logging.c
    ${SOURCE_DIR}/game/1fx/mvchat.c
    ${SOURCE_DIR}/game/1fx/threads.c
    ${SOURCE_DIR}/game/1fx/vote.c
    
    ${SOURCE_DIR}/game/1fx/clientmods/RPM.c
    ${SOURCE_DIR}/game/1fx/clientmods/ROCmod.c
)

add_library(${GAME_LIB} SHARED ${GAME_SOURCES})

target_link_libraries(${GAME_LIB} PRIVATE ${COMMON_LIBRARIES})

set_target_properties(${GAME_LIB} PROPERTIES
    OUTPUT_NAME "${GAME_LIB}${ARCH}"
    PREFIX ""
    ARCHIVE_OUTPUT_NAME ""
)



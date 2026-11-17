# native.cmake - the native runner's own build (phase 5 P6: docs/phases/P5-port.md), included by
# recomp/CMakeLists.txt; game_runner(... NATIVE ...) calls native_runner.
#
# The executable is the hand C in the headers' native branch (its game library, passed as SOURCES:
# the C, its bridge without the engine's dispatch stubs, the flow runtime, the arena, the exporter,
# the platform's native backend), the native machine (platform/native: the frame-synchronous machine,
# the runner's main, the input script, the SDL host) and from the engine only what the platform
# keeps (P6): the video model, the Z80 core, the sound chips and their mixer, the snapshottable
# fiber the game runs on. No 68000 core, interpreter, scheduler, dispatch, generated code or
# emulated work RAM is compiled in; `dkit genesis recomp scan` reads its objects and link map.
set(NATIVE_DIR "${CMAKE_CURRENT_LIST_DIR}")

function(native_runner TARGET)
    cmake_parse_arguments(NR "" "" "SOURCES;DEFS" ${ARGN})
    add_executable(${TARGET} ${NR_SOURCES}
        "${NATIVE_DIR}/machine.cpp"
        "${NATIVE_DIR}/sound2.cpp"
        "${NATIVE_DIR}/runner.cpp"
        "${NATIVE_DIR}/options.cpp"
        "${NATIVE_DIR}/enhance.cpp"
        "${NATIVE_DIR}/spr_host.cpp"
        "${NATIVE_DIR}/present.cpp"
        "${NATIVE_DIR}/widescreen.cpp"
        "${NATIVE_DIR}/ws_draw.cpp"
        "${NATIVE_DIR}/input_script.cpp"
        "${NATIVE_DIR}/host_sdl.cpp"
        "${CMAKE_SOURCE_DIR}/game/runtime/crash_win.cpp"
        "${RUNNER_ROOT}/fiber_compat.c"
        "${RUNNER_ROOT}/video/genesis_vdp.c"
        "${RUNNER_ROOT}/external/superzazu/z80.c"
        # the sound chips and their mixer (played runs; a headless run empties the queue each frame)
        "${RUNNER_ROOT}/audio.c"
        "${RUNNER_ROOT}/chip_trace.c"
        "${RUNNER_ROOT}/audio/event_queue.c"
        "${RUNNER_ROOT}/audio/mixer.c"
        "${RUNNER_ROOT}/audio/sn76489.c"
        "${RUNNER_ROOT}/audio/ym2612_ymfm.cpp"
        "${RUNNER_ROOT}/audio/fm_shadow.cpp"
        "${RUNNER_ROOT}/audio/observability.c"
        "${RUNNER_ROOT}/audio/audio_shadow.c"
        "${RUNNER_ROOT}/external/ymfm/src/ymfm_opn.cpp"
        "${RUNNER_ROOT}/external/ymfm/src/ymfm_ssg.cpp"
        "${RUNNER_ROOT}/external/ymfm/src/ymfm_adpcm.cpp"
        "${RUNNER_ROOT}/external/ymfm/src/ymfm_pcm.cpp")
    target_compile_definitions(${TARGET} PRIVATE DK_NATIVE=1 GENESIS_GAME_VERSION="${PROJECT_VERSION}"
        GENESIS_BATCHED_PLANES=$<STREQUAL:${GENESIS_VDP_PLANE_IMPL},BATCHED> ${NR_DEFS})
    target_include_directories(${TARGET} PRIVATE ${GAME_INCLUDE_DIRS} "${CMAKE_SOURCE_DIR}/game/include"
                               "${CMAKE_SOURCE_DIR}/platform" "${NATIVE_DIR}")
    target_link_libraries(${TARGET} PRIVATE SDL2::SDL2main SDL2::SDL2)
    if(MSVC)
        target_compile_options(${TARGET} PRIVATE /W3 /WX- /utf-8 /wd4102 /wd4996)
        # the game's fiber replaces the stack pointer without a shadow-stack switch (fiber_compat.h)
        target_link_options(${TARGET} PRIVATE "/CETCOMPAT:NO"
            "/MAP:$<TARGET_FILE_DIR:${TARGET}>/${TARGET}.map")
        add_custom_command(TARGET ${TARGET} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${RUNNER_ROOT}/external/SDL2/SDL2-2.28.5/lib/x64/SDL2.dll" "$<TARGET_FILE_DIR:${TARGET}>"
            COMMENT "Copying SDL2.dll")
    else()
        target_compile_definitions(${TARGET} PRIVATE _GNU_SOURCE)
        # no shadow stacks (the fiber's switch), and page-by-page probes of big frames on its stack
        target_compile_options(${TARGET} PRIVATE "$<$<COMPILE_LANGUAGE:C,CXX>:-fstack-clash-protection>")
        if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(x86_64|AMD64|amd64)$")
            target_compile_options(${TARGET} PRIVATE "$<$<COMPILE_LANGUAGE:C,CXX>:-fcf-protection=none>")
        endif()
        find_package(Threads REQUIRED)
        target_link_libraries(${TARGET} PRIVATE Threads::Threads)
    endif()
endfunction()

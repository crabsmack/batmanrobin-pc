cmake_minimum_required(VERSION 3.20)
project(batmanrobin_recomp VERSION 0.1.0 LANGUAGES C CXX)

# The Adventures of Batman and Robin, statically recompiled with segagenesisrecomp (written by `dkit genesis recomp init`;
# yours to extend: phase 4's stepped and verify runners, phase 5's native one, join here).
#
# Engine: the pinned submodule engine/ (dkit.toml [recomp] engine_url / engine_rev), or
# GENESIS_RECOMP_ROOT for engine development. game/ holds game.toml, the discovery files
# `dkit genesis recomp sync` writes from the disassembly, the GameSpec (spec.c) and the ROM
# (gitignored, copied in). Generated C is build output (build/generated/batmanrobin/): never edit it.

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

if(MSVC)
    # /MP: a project's own files compile in parallel (the generator's --parallel only runs
    # projects side by side). No identical-code folding: game functions are values the game
    # stores and compares; xmen2's release link merged two state functions with equal code.
    add_compile_options(/MP)
    add_link_options(/OPT:REF /OPT:NOICF)
    # Reproducible runners: the same sources give byte-identical executables (trace caches key
    # on the runner's SHA-1). /Brepro drops timestamps; game/runtime/reproducible replaces the
    # engine's build-info header, which changes at every commit.
    add_compile_options(/Brepro)
    add_link_options(/Brepro)
else()
    # GCC / Clang (the Linux build, P9): the semantics MSVC gives the C in practice, so the second
    # compiler finds real bugs instead of optimizing on assumptions the hand C was never written
    # for (kit playbook/porting.md point 8): wrapping signed arithmetic, no type-based alias
    # analysis, null checks kept
    add_compile_options(-fwrapv -fno-strict-aliasing -fno-delete-null-pointer-checks)
endif()

set(GENESIS_RECOMP_ROOT "" CACHE PATH "Optional segagenesisrecomp checkout (default: the engine submodule)")
if(GENESIS_RECOMP_ROOT)
    set(RECOMP_ROOT "${GENESIS_RECOMP_ROOT}")
else()
    set(RECOMP_ROOT "${CMAKE_SOURCE_DIR}/engine")
endif()
set(RUNNER_ROOT "${RECOMP_ROOT}/runner")

option(GEN_ENABLE_TRACE "Build the TCP cmd server + observability rings" ON)
if(GEN_ENABLE_TRACE)
    add_compile_definitions(GEN_ENABLE_TRACE=1)
endif()

if(WIN32)
    list(APPEND CMAKE_PREFIX_PATH "${RUNNER_ROOT}/external/SDL2/SDL2-2.28.5/cmake")
endif()
find_package(SDL2 REQUIRED)

include("${RECOMP_ROOT}/cmake/GenesisRecompGenerated.cmake")
genesisrecomp_runner_sources(GAME_RUNNER_SOURCES TRACE ${GEN_ENABLE_TRACE})

set(GAME_INCLUDE_DIRS
    "${RUNNER_ROOT}"
    "${RUNNER_ROOT}/include"
    "${RUNNER_ROOT}/external/ymfm/src"
    "${RUNNER_ROOT}/external/superzazu"
    "${RUNNER_ROOT}/external/clowncommon"
    "${RUNNER_ROOT}/video"
    "${RECOMP_ROOT}/recompiler/src"
    "${CMAKE_SOURCE_DIR}/game")
set(GAME_COMMON_DEFS INCLUDE_GENERATED_CODE=1 ENABLE_RECOMPILED_CODE=1 OWN_BACKEND=1
    GENESIS_GAME_VERSION="${PROJECT_VERSION}")

function(game_runner TARGET PORT)
    # NATIVE: the port's runner (phase 5 P6), none of the engine's runner: platform/native/native.cmake
    cmake_parse_arguments(GR "NATIVE" "" "DEFS;SOURCES;DEPENDS" ${ARGN})
    if(GR_NATIVE)
        native_runner(${TARGET} SOURCES ${GR_SOURCES} DEFS ${GR_DEFS})
        return()
    endif()
    add_executable(${TARGET} ${GAME_RUNNER_SOURCES} ${GR_SOURCES} "${CMAKE_SOURCE_DIR}/game/spec.c"
                   "${CMAKE_SOURCE_DIR}/game/runtime/crash_win.cpp")
    if(GR_DEPENDS)
        add_dependencies(${TARGET} ${GR_DEPENDS})
    endif()
    genesisrecomp_runner_target(${TARGET})
    target_compile_definitions(${TARGET} PRIVATE ${GAME_COMMON_DEFS} DEFAULT_DEBUG_PORT=${PORT} ${GR_DEFS})
    target_include_directories(${TARGET} BEFORE PRIVATE "${CMAKE_SOURCE_DIR}/game/runtime/reproducible")
    target_include_directories(${TARGET} PRIVATE ${GAME_INCLUDE_DIRS})
    target_link_libraries(${TARGET} PRIVATE SDL2::SDL2main SDL2::SDL2)
    if(WIN32)
        target_link_libraries(${TARGET} PRIVATE ws2_32 comdlg32)
    endif()
    if(NOT MSVC)
        # the engine's ram_views.c maps work RAM twice with memfd_create, a GNU extension glibc
        # declares only under _GNU_SOURCE (GCC 15 makes the implicit declaration an error: the
        # phase-5 Linux build, P9)
        target_compile_definitions(${TARGET} PRIVATE _GNU_SOURCE)
    endif()
    if(MSVC)
        target_compile_options(${TARGET} PRIVATE /W3 /WX- /utf-8 /wd4102 /wd4996)
        # the map names the frames of a crash's stack trace (game/runtime/crash_win.cpp); the
        # executable's bytes don't change
        target_link_options(${TARGET} PRIVATE "/MAP:$<TARGET_FILE_DIR:${TARGET}>/${TARGET}.map")
        add_custom_command(TARGET ${TARGET} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${RUNNER_ROOT}/external/SDL2/SDL2-2.28.5/lib/x64/SDL2.dll"
                "$<TARGET_FILE_DIR:${TARGET}>"
            COMMENT "Copying SDL2.dll")
    endif()
endfunction()

# The timed runner (phase 2): all generated code, cycle-exact, equal to the reference emulator
# every frame. Its generated code is one object library, so later runners can share it.
#
# game_code(LIB PREFIX [args...]): one generated code set as an object library (every file with
# the same command line, so MSBuild's /MP batches them: per-file flags made xmen2's builds
# serial, 59-95 min each). Extra args go to the recompiler (--untimed, --skip FILE).
function(game_code LIB PREFIX)
    cmake_parse_arguments(GC "" "" "ARGS;DEPENDS" ${ARGN})
    genesisrecomp_add_generated_sources(_gen ${PREFIX} "${CMAKE_SOURCE_DIR}/game" batmanrobin.bin
        EXTRA_ARGS ${GC_ARGS} EXTRA_DEPENDS ${GC_DEPENDS})
    add_library(${LIB} OBJECT ${_gen})
    add_dependencies(${LIB} ${_gen_TARGET})
    target_compile_definitions(${LIB} PRIVATE ${GAME_COMMON_DEFS})
    target_include_directories(${LIB} PRIVATE ${GAME_INCLUDE_DIRS})
    if(MSVC)
        target_compile_options(${LIB} PRIVATE /W3 /WX- /utf-8 /wd4102 /wd4996)
    endif()
endfunction()

game_code(batmanrobin_timed_code batmanrobin)
game_runner(batmanrobin_recomp 4382 SOURCES $<TARGET_OBJECTS:batmanrobin_timed_code>)

# Phase 4 (docs/phases/P4-decomp.md): the frame-synchronous stepped runner over untimed
# generated code (stepped-gen: the reference every hand-C build is compared with at sync points).
game_code(batmanrobin_untimed_code batmanrobing ARGS --untimed --prefix batmanrobing)
game_runner(batmanrobin_stepped_gen 4385 DEFS DK_STEPPED=1 SOURCES $<TARGET_OBJECTS:batmanrobin_untimed_code>)

# The hand-C runners: game/c/<ADDR>.c through the bridge `dkit genesis recomp bridge` writes into
# the build dir (`dkit genesis recomp build` runs it first): stepped = stepped-gen's machine with
# the C in place of its functions' generated code (D2); verify = the timed runner with the C as
# shadow differential tests (D4: game/runtime/verify.cpp). One object library per bridge, every
# file with the same command line (/MP).
function(game_handc LIB BRIDGE)
    # extra sources after BRIDGE (or SOURCES ...); DEFS: the library's own definitions (the
    # native one's DK_NATIVE: the headers' native branch); RUNTIME: the runtime under the C
    # (default handc_rt.cpp, the 68000's; the native one's is the flow runtime)
    # PLATFORM: the platform layer's backend (phase 5 P4: platform/genesis_platform.h), the object
    # the C's hardware accesses go through: bus (default) = the engine's emulated bus
    # (platform/platform_bus.cpp); P6's native machine adds its own backend file here
    cmake_parse_arguments(GH "" "PLATFORM" "DEFS;SOURCES;RUNTIME" ${ARGN})
    if(NOT GH_PLATFORM)
        set(GH_PLATFORM bus)
    endif()
    include("${BRIDGE}/handc.cmake")   # GAME_HANDC_SOURCES, GAME_BRIDGE_SOURCES
    if(NOT GAME_BRIDGE_SOURCES)
        set(GAME_BRIDGE_SOURCES "${BRIDGE}/bridge.cpp")
    endif()
    if(GH_PLATFORM STREQUAL "native")
        # the native machine has no engine dispatch table: the bridge's func_ADDR stubs for it go
        list(FILTER GAME_BRIDGE_SOURCES EXCLUDE REGEX "engine_stubs\\.cpp$")
    endif()
    if(NOT GH_RUNTIME)
        set(GH_RUNTIME "${CMAKE_SOURCE_DIR}/game/runtime/handc_rt.cpp")
    endif()
    add_library(${LIB} OBJECT ${GAME_HANDC_SOURCES} ${GAME_BRIDGE_SOURCES} ${GH_RUNTIME}
                "${CMAKE_SOURCE_DIR}/platform/platform_${GH_PLATFORM}.cpp" ${GH_UNPARSED_ARGUMENTS} ${GH_SOURCES})
    target_compile_definitions(${LIB} PRIVATE ${GAME_COMMON_DEFS} ${GH_DEFS})
    target_include_directories(${LIB} PRIVATE ${GAME_INCLUDE_DIRS} "${CMAKE_SOURCE_DIR}/game/include"
                               "${CMAKE_SOURCE_DIR}/platform")
    if(MSVC)
        target_compile_options(${LIB} PRIVATE /W3 /WX- /utf-8 /wd4102 /wd4996 /permissive-)
    endif()
endfunction()

set(BRIDGE_STEPPED "${CMAKE_BINARY_DIR}/bridge-stepped")
if(EXISTS "${BRIDGE_STEPPED}/handc.cmake")
    game_code(batmanrobin_stepped_code batmanrobins ARGS --untimed --prefix batmanrobins --skip "${BRIDGE_STEPPED}/skip.txt"
              DEPENDS "${BRIDGE_STEPPED}/skip.txt")
    game_handc(batmanrobin_handc_stepped "${BRIDGE_STEPPED}")
    game_runner(batmanrobin_stepped 4383 DEFS DK_STEPPED=1 DK_HANDC=1
                SOURCES $<TARGET_OBJECTS:batmanrobin_stepped_code> $<TARGET_OBJECTS:batmanrobin_handc_stepped>)
endif()

set(BRIDGE_VERIFY "${CMAKE_BINARY_DIR}/bridge-verify")
if(EXISTS "${BRIDGE_VERIFY}/handc.cmake")
    game_handc(batmanrobin_handc_verify "${BRIDGE_VERIFY}" "${CMAKE_SOURCE_DIR}/game/runtime/verify.cpp")
    game_runner(batmanrobin_verify 4384 DEFS DK_VERIFY=1 DK_HANDC=1
                SOURCES $<TARGET_OBJECTS:batmanrobin_timed_code> $<TARGET_OBJECTS:batmanrobin_handc_verify>)
endif()

# Phase 5 (docs/phases/P5-port.md): the native port's runner. The hand C compiled in the headers'
# native branch (DK_NATIVE; every source keeps compiling in both modes), through its own bridge
# (bridge-native), on the native machine (platform/native, P6): the frame-synchronous machine of the
# stepped runners rebuilt without the 68000 (rulings R3, R5), the engine's video model, Z80 and sound
# chips, nothing of its runtime. Its sync rows come only from the exporter (game/runtime/export.cpp):
# the 68000 work-RAM / VRAM / CRAM / VSRAM image of the game state at each sync point. The C's
# control flow runs on the native flow runtime (ruling R8: game/runtime/flow.cpp) hosted by
# platform/native/flow_native.cpp; hint_raster.cpp is the H-int's RAM code in C (R3); arena.cpp the
# native arena, all game RAM under ruling R1 (dk_mem.h's native branch). `dkit genesis recomp scan`
# reads its game objects (the native handc library's) and its link map (P3-P6).
include("${CMAKE_SOURCE_DIR}/platform/native/native.cmake")
set(BRIDGE_NATIVE "${CMAKE_BINARY_DIR}/bridge-native")
if(NOT EXISTS "${BRIDGE_NATIVE}/handc.cmake" AND EXISTS "${CMAKE_SOURCE_DIR}/bridge-native/handc.cmake")
    # the public export's bridge (dkit export, kit playbook/publishing.md): its build has no kit to
    # write one into the build tree; the working repo has none, so it always builds its own
    set(BRIDGE_NATIVE "${CMAKE_SOURCE_DIR}/bridge-native")
endif()
if(EXISTS "${BRIDGE_NATIVE}/handc.cmake")
    game_handc(batmanrobin_handc_native "${BRIDGE_NATIVE}" DEFS DK_NATIVE=1 PLATFORM native
               RUNTIME "${CMAKE_SOURCE_DIR}/game/runtime/flow.cpp" "${CMAKE_SOURCE_DIR}/game/runtime/hint_raster.cpp"
                       "${CMAKE_SOURCE_DIR}/platform/native/flow_native.cpp"
                       "${CMAKE_SOURCE_DIR}/platform/native/game_mem.cpp"
               SOURCES "${CMAKE_SOURCE_DIR}/game/runtime/export.cpp" "${CMAKE_SOURCE_DIR}/game/runtime/arena.cpp")
    game_runner(batmanrobin_native 4386 NATIVE SOURCES $<TARGET_OBJECTS:batmanrobin_handc_native>)
endif()

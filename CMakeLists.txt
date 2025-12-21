cmake_minimum_required(VERSION 3.16)
project(tty-snd C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
set(CMAKE_POLICY_VERSION_MINIMUM "3.5")

add_compile_definitions(HAS_RAYLIB)

# --- Dependencies ---
include(FetchContent)

# Fetch Raylib
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_TESTS OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG        5.0
)
FetchContent_MakeAvailable(raylib)

# Fetch OpenAL Soft (only for headers)
set(ALSOFT_EXAMPLES OFF CACHE BOOL "" FORCE)
set(ALSOFT_TESTS OFF CACHE BOOL "" FORCE)
set(ALSOFT_UTILS OFF CACHE BOOL "" FORCE)
set(ALSOFT_INSTALL OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
    openal_soft
    GIT_REPOSITORY https://github.com/kcat/openal-soft.git
    GIT_TAG        master
)
FetchContent_MakeAvailable(openal_soft)
include_directories(${openal_soft_SOURCE_DIR}/include)

# --- Platform Compatibility ---
if(WIN32)
    # Enable Windows compatibility headers
    include_directories(${CMAKE_SOURCE_DIR}/win32)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
else()
    # On Linux, link math library
    link_libraries(m)
endif()

# --- Sources ---
set(COMMON_SOURCES
    util.c
    simple_wav.c
    f80.c
    alad.c
)

# --- Targets ---

# tty-snd-wav
add_executable(tty-snd-wav wav_main.c wav.c ${COMMON_SOURCES})
target_link_libraries(tty-snd-wav PRIVATE raylib)

# tty-snd-fft
add_executable(tty-snd-fft fft_main.c fft.c ${COMMON_SOURCES})
target_link_libraries(tty-snd-fft PRIVATE raylib)

# tty-snd-ifft
add_executable(tty-snd-ifft ifft_main.c fft.c ${COMMON_SOURCES})
target_link_libraries(tty-snd-ifft PRIVATE raylib)

# tty-snd-graph
add_executable(tty-snd-graph graph_main.c ${COMMON_SOURCES})
target_compile_definitions(tty-snd-graph PRIVATE HAS_RAYLIB)
if(WIN32)
    target_link_libraries(tty-snd-graph PRIVATE raylib winmm gdi32 user32 shell32 opengl32)
else()
    target_link_libraries(tty-snd-graph PRIVATE raylib m dl pthread GL X11)
endif()

# tty-snd-peaks
add_executable(tty-snd-peaks cutoff_intervals.c peak_main.c ${COMMON_SOURCES})
target_link_libraries(tty-snd-peaks PRIVATE raylib)

# tty-snd-mic-src
add_executable(tty-snd-mic-src mic_src_main.c ${COMMON_SOURCES})
target_link_libraries(tty-snd-mic-src PRIVATE raylib)

# tty-snd-stretch
add_executable(tty-snd-stretch stretch_main.c ${COMMON_SOURCES})
target_link_libraries(tty-snd-stretch PRIVATE raylib)

# tty-snd-play
add_executable(tty-snd-play play_main.c ${COMMON_SOURCES})
target_link_libraries(tty-snd-play PRIVATE raylib)

# tty-snd-reduce
add_executable(tty-snd-reduce reduce_main.c ${COMMON_SOURCES})
target_link_libraries(tty-snd-reduce PRIVATE raylib)

# tty-snd-cmplx
add_executable(tty-snd-cmplx complexify_main.c ${COMMON_SOURCES})
target_link_libraries(tty-snd-cmplx PRIVATE raylib)

# tty-snd-peak-print
add_executable(tty-snd-peak-print peak_dbg_main.c ${COMMON_SOURCES})
target_link_libraries(tty-snd-peak-print PRIVATE raylib)

# tty-snd-change_rolloff_velocity
add_executable(tty-snd-change_rolloff_velocity change_rolloff_v_main.c ${COMMON_SOURCES})
target_link_libraries(tty-snd-change_rolloff_velocity PRIVATE raylib)

# tty-snd-change_rolloff_slope
add_executable(tty-snd-change_rolloff_slope slope_main.c ${COMMON_SOURCES})
target_link_libraries(tty-snd-change_rolloff_slope PRIVATE raylib)

# tty-snd-bfilter
add_executable(tty-snd-bfilter boxfilter_main.c ${COMMON_SOURCES})
target_link_libraries(tty-snd-bfilter PRIVATE raylib)

# tty-snd-wndw
add_executable(tty-snd-wndw windowing_main.c ${COMMON_SOURCES})
target_link_libraries(tty-snd-wndw PRIVATE raylib)

# Exclude marple-alg_2.c on Windows
if(NOT WIN32)
    add_executable(tty-snd-nftest newformant_test_main.c lpc.c root.c marple-alg_2.c informant_algs.c ${COMMON_SOURCES})
    target_link_libraries(tty-snd-nftest PRIVATE raylib)
endif()

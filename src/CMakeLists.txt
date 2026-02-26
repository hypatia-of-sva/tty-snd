cmake_minimum_required(VERSION 3.16)
project(tty-snd C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
set(CMAKE_POLICY_VERSION_MINIMUM "3.5")
add_compile_options(-g -Wall -Wextra)

#add_compile_definitions(HAS_RAYLIB)

# --- Dependencies ---
#include(FetchContent)

# Fetch Raylib
#set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
#set(BUILD_TESTS OFF CACHE BOOL "" FORCE)
#FetchContent_Declare(
#    raylib
#    GIT_REPOSITORY https://github.com/raysan5/raylib.git
#    GIT_TAG        5.0
#)
#FetchContent_MakeAvailable(raylib)

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
    alad.c
    wav.c
)
include_directories("../deps-include")

find_package(GSL REQUIRED)  
#target_link_libraries(Unitsv1 GSL::gsl GSL::gslcblas)

# --- Targets ---

# tty-snd-wav
add_executable(tty-snd-wav wav_main.c ${COMMON_SOURCES})
add_executable(tty-snd-complexify complexify_main.c ${COMMON_SOURCES})
add_executable(tty-snd-reduce_real reduce_real_main.c ${COMMON_SOURCES})
add_executable(tty-snd-reduce_imag reduce_imag_main.c ${COMMON_SOURCES})
add_executable(tty-snd-reduce_cabs reduce_cabs_main.c ${COMMON_SOURCES})
add_executable(tty-snd-fft fft.c fft_main.c ${COMMON_SOURCES})
add_executable(tty-snd-ifft fft.c ifft_main.c ${COMMON_SOURCES})






# add_executable(tty-snd-wav wav_main.c wav.c ${COMMON_SOURCES})
# target_link_libraries(tty-snd-wav PRIVATE raylib)


# tty-snd-graph
#add_executable(tty-snd-graph graph_main.c ${COMMON_SOURCES})
#target_compile_definitions(tty-snd-graph PRIVATE HAS_RAYLIB)
#if(WIN32)
#    target_link_libraries(tty-snd-graph PRIVATE raylib winmm gdi32 user32 shell32 opengl32)
#else()
#    target_link_libraries(tty-snd-graph PRIVATE raylib m dl pthread GL X11)
#endif()

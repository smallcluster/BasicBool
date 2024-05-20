cmake_minimum_required(VERSION 3.9)

project(BasicBool)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_EXE_LINKER_FLAGS -static)

# set output directories for all builds (Debug, Release, etc.)
foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
    string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG )
    set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${PROJECT_SOURCE_DIR} )
endforeach( OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES )

add_executable(
        ${PROJECT_NAME}
        src/main.cpp

        # PLATFORM
        src/platform/platform.hpp
        src/platform/platform_win32.cpp
        src/platform/platform_linux.cpp

        # CORE
        src/core/logger.hpp
        src/core/defines.hpp
        src/core/math.hpp

        # RENDER
        src/render/backend.hpp
        src/render/backend.cpp
        src/render/shapes.hpp
        src/render/shapes.cpp
        src/render/text.hpp
        src/render/text.cpp

        # NODE
        src/node/node_system.hpp
        src/node/node_system.cpp
        src/node/nodes.hpp
        src/node/look_and_feel.hpp

        # GUI
        src/gui/gui.hpp)

target_include_directories(${PROJECT_NAME} PUBLIC src/)

#set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} --my-debug-flags")
target_compile_definitions(${PROJECT_NAME} PUBLIC "$<$<CONFIG:Debug>:BUILD_DEBUG>")


# --- DEPS ---

# OpenGL
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE ${OPENGL_LIBRARIES})

# glad
add_library(glad STATIC
        external/glad/src/glad.c
        external/glad/include/glad/glad.h
        external/glad/include/KHR/khrplatform.h)
target_include_directories(glad PUBLIC external/glad/include)
target_link_libraries(${PROJECT_NAME} PRIVATE glad)

# OPENMP
find_package(OpenMP REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE OpenMP::OpenMP_CXX)

target_include_directories(${PROJECT_NAME} PUBLIC external/stb)
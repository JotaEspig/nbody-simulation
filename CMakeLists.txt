cmake_minimum_required(VERSION 2.8.5...3.27.5)
project(nbody-simulation)

# Debug mode
set(FLAGS "-Wall")
if (CMAKE_COMPILER_IS_GNUXX)
    set(FLAGS "${FLAGS} -Wextra")
endif (CMAKE_COMPILER_IS_GNUXX)
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif ()

# CMake settings
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${FLAGS}")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${FLAGS} -g3 -fsanitize=address,undefined")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${FLAGS} -O3")

# Find packages
set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm REQUIRED)
find_package(assimp CONFIG REQUIRED)
find_package(Doxygen OPTIONAL_COMPONENTS dot)

add_library(axolote SHARED IMPORTED)
set_target_properties(axolote PROPERTIES
    IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/external/lib/axolote/axolote.dll"
    IMPORTED_IMPLIB "${CMAKE_SOURCE_DIR}/external/lib/axolote/axolote.lib"
)
add_dependencies(axolote glfw)
add_dependencies(axolote assimp::assimp)


# =-=-=-=-=-=-= DOCS =-=-=-=-=-=-=
if (DOXYGEN_FOUND)
    message("Doxygen found. You can use `make docs`")

    add_custom_target(
        docs
        COMMAND ${DOXYGEN_EXECUTABLE}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        VERBATIM
    )
else (DOXYGEN_FOUND)
    message("Doxygen not found. You won't be able to use `make docs`")
endif (DOXYGEN_FOUND)


# =-=-=-=-=-=-= EXECUTABLE =-=-=-=-=-=-=
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# Source files
set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)
set(
    SOURCE_FILES
    ${SOURCE_DIR}/app.cpp
    ${SOURCE_DIR}/celestial_body.cpp
    ${SOURCE_DIR}/celestial_body_system.cpp
    ${SOURCE_DIR}/main.cpp
    ${SOURCE_DIR}/octree.cpp
    ${SOURCE_DIR}/sphere.cpp
)

# Adding executables
add_executable(nbody-simulation ${SOURCE_FILES})
add_dependencies(nbody-simulation axolote)
target_compile_definitions(nbody-simulation PUBLIC GLAD_GLAPI_EXPORT)

# Libraries
target_link_directories(
    nbody-simulation
    PRIVATE
    ${CMAKE_SOURCE_DIR}/external/lib/axolote
)
target_link_libraries(
    nbody-simulation
    PRIVATE
    axolote
    ${OPENGL_LIBRARIES} glfw assimp::assimp
)

# Header files
target_include_directories(
    nbody-simulation
    PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/external/include
)

if (WIN32 OR MSVC)
    add_custom_command(TARGET nbody-simulation POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        $<TARGET_RUNTIME_DLLS:nbody-simulation>
        $<TARGET_FILE_DIR:nbody-simulation>
        COMMAND_EXPAND_LISTS
    )

    # Ensure that necessary DLLs are copied to the output directory
    add_custom_command(TARGET nbody-simulation POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/external/lib/axolote
        $<TARGET_FILE_DIR:nbody-simulation>
        COMMAND_EXPAND_LISTS
    )
endif ()

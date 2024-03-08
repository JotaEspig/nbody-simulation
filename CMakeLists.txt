cmake_minimum_required(VERSION 2.8.5...3.27.5)
project(solar-system-3d)

# CMake settings
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

# Find packages
set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(Doxygen OPTIONAL_COMPONENTS dot)


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
    ${SOURCE_DIR}/solar_system.cpp
    ${SOURCE_DIR}/main.cpp
)

# Adding executables
add_executable(solar-system-3d ${SOURCE_FILES})

# Libraries
target_link_directories(
    solar-system-3d
    PRIVATE
    ${CMAKE_SOURCE_DIR}/external/lib/axolote
)
target_link_libraries(
    solar-system-3d
    PRIVATE
    axolote
    ${OPENGL_LIBRARIES} glfw stdc++
)

# Header files
target_include_directories(
    solar-system-3d
    PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/external/include
)

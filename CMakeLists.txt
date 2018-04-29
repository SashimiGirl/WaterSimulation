# Project definition
cmake_minimum_required(VERSION 3.1)
project(FluidyFun)

option(BUILD_DEBUG     "Build with debug settings"    OFF)
option(ENABLE_TESTS    "Enable testing"               OFF)


####################
# Build on Windows #
####################
if(WIN32)

  if(MSVC)

    set(MSVC_CXX_FLAGS "-std=gnu++11")

    if(BUILD_DEBUG)
        set(CMAKE_BUILD_TYPE Debug)
    else(BUILD_DEBUG)
    endif(BUILD_DEBUG)

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${MSVC_CXX_FLAGS}")

  endif(MSVC)

  if(MINGW)

    set(MSVC_CXX_FLAGS "-std=gnu++11")

    if(BUILD_DEBUG)
        set(CMAKE_BUILD_TYPE Debug)
    else(BUILD_DEBUG)
    endif(BUILD_DEBUG)

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${MSVC_CXX_FLAGS}")

  endif(MINGW)

endif(WIN32)


# Source files
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(SOURCES 

${SRC_DIR}/main.cpp
${SRC_DIR}/Simulator.cpp
${SRC_DIR}/SolidSphere.cpp
${SRC_DIR}/MeshObject.cpp
)

# Executable definition and properties
add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE "${SRC_DIR}")
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 11)

add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory 
      ${SRC_DIR}/shaders $<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders)


# GLFW
#find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)
if(OPENGL_FOUND)
 target_include_directories(${PROJECT_NAME} PRIVATE ${OPENGL_INCLUDE_DIR})
 target_link_libraries(${PROJECT_NAME} ${OPENGL_gl_LIBRARY})
endif(OPENGL_FOUND)

# glfw
set(GLFW_DIR "${LIB_DIR}/glfw")
set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "Build the GLFW example programs")
set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "Build the GLFW test programs")
set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "Build the GLFW documentation")
set(GLFW_INSTALL OFF CACHE INTERNAL "Generate installation target")
add_subdirectory("${GLFW_DIR}")
target_link_libraries(${PROJECT_NAME} "glfw" "${GLFW_LIBRARIES}")
target_include_directories(${PROJECT_NAME} PRIVATE "${GLFW_DIR}/include")
target_compile_definitions(${PROJECT_NAME} PRIVATE "GLFW_INCLUDE_NONE")

# glad
set(GLAD_DIR "${LIB_DIR}/glad")
add_library("glad" "${GLAD_DIR}/src/glad.c")
target_include_directories("glad" PRIVATE "${GLAD_DIR}/include")
target_include_directories(${PROJECT_NAME} PRIVATE "${GLAD_DIR}/include")
target_link_libraries(${PROJECT_NAME} "glad" "${CMAKE_DL_LIBS}")


# assimp
set(ASSIMP_DIR "${LIB_DIR}/assimp")
add_subdirectory("${ASSIMP_DIR}")
target_include_directories(${PROJECT_NAME} PRIVATE "${ASSIMP_DIR}/include")
target_link_libraries(${PROJECT_NAME} "assimp")





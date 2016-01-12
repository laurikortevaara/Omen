######################################################################
# Omen Game engine project
#
# Lauri Kortevaara (2015)
# lauri.kortevaara@gmail.com
#
# https://github.com/laurikortevaara/Omen
#
######################################################################
cmake_minimum_required(VERSION 3.0)
project(Omen)

######################################################################
# Set the working directory for debugging
######################################################################
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

######################################################################
# FORCE OS X 10.8 DEPLOYMENT TARGET ON OS X TO SUPPORT GLUT
######################################################################
if (APPLE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.8")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__gl_h_ -DGL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED")
endif (APPLE)

######################################################################
# Compiler flags:
# - C++14 - Use C++ standard 14
# - libc++ - Use libc++ as the C++ standard library (http://libcxx.llvm.org/)
######################################################################
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED on)
set(MALLOC_CHECK_ 2)
set(ENV{MALLOC_CHECK_} "2")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -stdlib=libc++")
set(CMAKE_C_FLAGS_DEBUG "-D__DEBUG")

######################################################################
# GLEW - OpenGL extension wrangler library
######################################################################
find_package(GLEW REQUIRED)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I${GLEW_INCLUDE_DIRS}")

######################################################################
# GLFW - Multiplatform OpenGL Window management library
######################################################################
set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_TESTS OFF)
add_subdirectory(${CMAKE_SOURCE_DIR}/thirdparty/glfw)

######################################################################
# STB Image loading library
######################################################################
include_directories(${CMAKE_SOURCE_DIR}/thirdparty/stb)

######################################################################
# Cross platform message box
######################################################################
add_subdirectory(${CMAKE_SOURCE_DIR}/thirdparty/boxer)
include_directories(${CMAKE_SOURCE_DIR}/thirdparty/boxer/include)

######################################################################
# OpenGL library
######################################################################
find_library(OPENGL OpenGL)
if (NOT OPENGL)
    message(FATAL_ERROR "OpenGL not found")
endif ()

######################################################################
# Google Test library (WIP)
######################################################################
add_subdirectory(${CMAKE_SOURCE_DIR}/thirdparty/googletest)

######################################################################
# FREETYPE
######################################################################
add_subdirectory(${CMAKE_SOURCE_DIR}/thirdparty/freetype)
include_directories(${CMAKE_SOURCE_DIR}/thirdparty/freetype/include)
find_library(FREETYPE FreeType)
if (NOT FREETYPE)
    message(FATAL_ERROR "FreeType not found")
endif ()


######################################################################
# BULLET PHYSICS
######################################################################
find_package(Bullet REQUIRED)
if(NOT BULLET_FOUND)
    message(FATAL_ERROR "FreeType not found")
endif ()

include_directories(${BULLET_INCLUDE_DIRS})

######################################################################
# tinydir
######################################################################
include_directories(${CMAKE_SOURCE_DIR}/thirdparty/tinydir)


######################################################################
# Os x specific libraries/frameworks
######################################################################
if (APPLE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.11")
    find_library(CARBON Carbon)
    if (NOT CARBON)
        message(FATAL_ERROR "Carbon not found")
    endif ()

    find_library(CORE_FOUNDATION CoreFoundation)
    if (NOT CORE_FOUNDATION)
        message(FATAL_ERROR "CoreFoundation not found")
    endif ()

    find_library(COCOA Cocoa)
    if (NOT COCOA)
        message(FATAL_ERROR "Cocoa not found")
    endif ()

    find_library(IOKIT IOKit)
    if (NOT IOKIT)
        message(FATAL_ERROR "IOKit not found")
    endif ()

    find_library(CORE_VIDEO CoreVideo)
    if (NOT CORE_VIDEO)
        message(FATAL_ERROR "CoreVideo not found")
    endif ()

    SET(LIBRARIES Boxer glfw ${GLEW} ${OPENGL} ${CARBON} ${COCOA} ${CORE_VIDEO} ${IOKIT} ${CORE_FOUNDATION} ${FREETYPE} ${BULLET_LIBRARIES})
endif (APPLE)


######################################################################
# Windows platform specific libs
######################################################################
if (WINDOWS)
    SET(LIBRARIES Boxer glfw ${GLEW} ${OPENGL} ${FREETYPE} ${BULLET_LIBRARIES})
endif (WINDOWS)


######################################################################
# Define the source files and the executable
######################################################################
set(SOURCE_FILES main.cpp Model.cpp Model.h Mesh.cpp Mesh.h Triangle.cpp Triangle.h Vertex.cpp Vertex.h VertexArrayObject.cpp VertexArrayObject.h VertexBufferObject.cpp VertexBufferObject.h Scene.cpp Scene.h GL_error.h Engine.cpp Engine.h Camera.cpp Camera.h Window.cpp Window.h Signal.h Shader.cpp Shader.h Texture.cpp Texture.h Camera.cpp Camera.h Entity.cpp Entity.h component/Component.cpp component/Component.h system/System.cpp system/System.h GameObject.cpp GameObject.h component/Transform.cpp component/Transform.h system/PhysicsSystem.cpp system/PhysicsSystem.h system/GraphicsSystem.cpp system/GraphicsSystem.h system/InputSystem.cpp system/InputSystem.h component/KeyboardInput.cpp component/MouseInput.cpp component/KeyboardInput.h component/JoystickInput.cpp component/JoystickInput.h MathUtils.h TextRenderer.cpp TextRenderer.h Material.cpp Material.h utils.h utils.cpp WavefrontLoader.cpp WavefrontLoader.h MD3Loader.cpp MD3Loader.h BoundingBox.cpp BoundingBox.h BoundingVolume.cpp BoundingVolume.h component/Picker.cpp component/Picker.h component/CameraController.cpp component/CameraController.h system/CoreSystem.cpp system/CoreSystem.h Light.cpp Light.h PointLight.cpp PointLight.h Sky.cpp Sky.h component/Transformer.cpp component/Transformer.h)
add_executable(Omen ${SOURCE_FILES})

######################################################################
# Set the target link libraries
######################################################################
target_link_libraries(Omen ${LIBRARIES})

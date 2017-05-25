######################################################################
# omen Game engine project
#
# Lauri Kortevaara (2015,2016)
# lauri.kortevaara@gmail.com
#
# https://github.com/laurikortevaara/omen
#
######################################################################
cmake_minimum_required(VERSION 3.3)
project(Omen)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)
set(CMAKE_CXX_STANDARsD 17)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Build the GLFW example programs")
set(GLFW_BUILD_TESTS OFF CACHE BOOL "Build the GLFW test programs")

set(BUILD_CPU_DEMOS OFF CACHE BOOL "Build original Bullet CPU demos")
set(BUILD_BULLET2_DEMOS OFF CACHE BOOL "Set when you want to build the Bullet 2 demos")
set(BUILD_BULLET3_DEMOS OFF CACHE BOOL "Set when you want to build the Bullet 3 demos")
set(GLFW_BUILD_DOCS OFF CACHE BOOL "Build GLFW Docs")
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Build GLFW Examples")
set(GLFW_BUILD_TESTS OFF CACHE BOOL "Build GLFW Unit Tests")
set(GLFW_INSTALL OFF CACHE BOOL "Install GLFW")
set(USE_GRAPHICAL_BENCHMARK OFF CACHE BOOL "Use Graphical Benchmark")
set(BUILD_CPU_DEMOS OFF CACHE BOOL "Build CPU Demos")
set(BUILD_EXTRAS OFF CACHE BOOL "Build extras")
set(BUILD_BULLET3 OFF CACHE BOOL "Build Bullet3")
set(BUILD_OPENGL3_DEMOS OFF CACHE BOOL "Build OpenGL 3 Demos")
set(BUILD_BULLET2_DEMOS OFF CACHE BOOL "Build Bullet 2 Demos")
set(BUILD_UNIT_TESTS OFF CACHE BOOL "Build Unit tests")

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

set(CMAKE_C_FLAGS_DEBUG "-D__DEBUG")
set(CMAKE_GENERATOR_PLATFORM Visual Studio 14 2015)

######################################################################
# GLEW - OpenGL extension wrangler library
######################################################################
if(APPLE)
    set(GLEW_INCLUDE_DIR $(CMAKE_SOURCE_DIR)/thirdparty/glew/include)
    set(GLEW_LIBRARY $(CMAKE_SOURCE_DIR)/thirdparty/glew/lib/Release/Win32)
    find_package(GLEW REQUIRED)
    include_directories($(CMAKE_SOURCE_DIR)/thirdparty/glew/include)
    add_definitions(-DGLEW_STATIC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I${GLEW_INCLUDE_DIRS}")
else()
    set(GLEW_INCLUDE_DIR $(CMAKE_SOURCE_DIR)/thirdparty/glew/include)
    set(GLEW_LIBRARY ${CMAKE_SOURCE_DIR}/thirdparty/glew/lib/Release/Win32/glew32s.lib)
    find_package(GLEW REQUIRED)
    if (GLEW_FOUND)
        include_directories(${GLEW_INCLUDE_DIRS})
        link_libraries(${GLEW_LIBRARIES})
    endif()
    include_directories($(CMAKE_SOURCE_DIR)/thirdparty/glew/include)
    add_definitions(-DGLEW_STATIC)
endif(APPLE)
include_directories(thirdparty/glew/include)


######################################################################
# GLFW - Multiplatform OpenGL Window management library
######################################################################
include_directories(${CMAKE_SOURCE_DIR}/thirdparty/glfw/include)
if (APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -stdlib=libc++")
    set(GLFW_BUILD_EXAMPLES OFF)
    set(GLFW_BUILD_TESTS OFF)
    add_subdirectory(${CMAKE_SOURCE_DIR}/thirdparty/glfw)
    include_directories(${CMAKE_SOURCE_DIR}/thirdparty/glfw/include)
else()
    add_subdirectory(${CMAKE_SOURCE_DIR}/thirdparty/glfw)
endif()


######################################################################
# STB Image loading library
######################################################################
include_directories(${CMAKE_SOURCE_DIR}/thirdparty/stb)

######################################################################
# Cross platform message box
######################################################################
#add_subdirectory(${CMAKE_SOURCE_DIR}/thirdparty/boxer)
include_directories(${CMAKE_SOURCE_DIR}/thirdparty/boxer/include)

######################################################################
# OpenGL library
######################################################################
#find_library(OPENGL OpenGL)
#if (NOT OPENGL)
#    message(FATAL_ERROR "OpenGL not found")
#endif ()

######################################################################
# Google Test library (WIP)
######################################################################
#add_subdirectory(${CMAKE_SOURCE_DIR}/thirdparty/googletest)

######################################################################
# FREETYPE
######################################################################
add_subdirectory(${CMAKE_SOURCE_DIR}/thirdparty/freetype)
include_directories(${CMAKE_SOURCE_DIR}/thirdparty/freetype/include)
#FREETYPE_DIR
#find_library(FREETYPE FreeType)
#if (NOT FREETYPE)
#    message(FATAL_ERROR "FreeType not found")
#endif ()

######################################################################
# OpenVR SDK
######################################################################
include_directories(${CMAKE_SOURCE_DIR}/thirdparty/openvr/headers)
set(OPENVR_SDK_LIBRARY ${CMAKE_SOURCE_DIR}/thirdparty/openvr/lib/win32/openvr_api.lib)

######################################################################
# Mono SDK
######################################################################
include_directories(${CMAKE_SOURCE_DIR}/thirdparty/mono/include/mono-2.0)
set(MONO_LIBRARY ${CMAKE_SOURCE_DIR}/thirdparty/mono/lib/mono-2.0-sgen.lib)

######################################################################
# BULLET PHYSICS
######################################################################
#set(BULLET_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/thirdparty/bullet/include)
#find_package(Bullet REQUIRED)
#if(NOT BULLET_FOUND)
#    message(FATAL_ERROR "Bullet not found")
#endif ()

add_subdirectory(${CMAKE_SOURCE_DIR}/thirdparty/bullet)
#find_package(Bullet REQUIRED)
#if(NOT BULLET_FOUND)
#    message(FATAL_ERROR "Bullet not found")
#endif ()
include_directories(${BULLET_INCLUDE_DIRS})
include_directories(thirdparty/bullet/src)

######################################################################
# LIBPNG
######################################################################
if (APPLE)
  find_package(Png REQUIRED)
  if(NOT PNG_FOUND)
      message(FATAL_ERROR "Png not found")
  endif (NOT PNG_FOUND)
else(APPLE)
  set(ZLIB_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/thirdparty/zlib)
  set(ZLIB_LIBRARY ${CMAKE_SOURCE_DIR}/thirdparty/zlib/lib/zlibstaticd.lib)
  set(PNG_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/thirdparty/libpng)
  set(PNG_LIBRARY ${CMAKE_SOURCE_DIR}/thirdparty/libpng/lib/libpng16_staticd.lib)
  #add_subdirectory(${CMAKE_SOURCE_DIR}/thirdparty/libpng)
endif(APPLE)

include_directories(${BULLET_INCLUDE_DIRS})


######################################################################
# FBXSDK
######################################################################
find_package(FbxSdk REQUIRED)
if(NOT FBX_INCLUDE_DIR)
    message(FATAL_ERROR "FBX SDK not found: " ${FBX_INCLUDE_DIR})
endif (NOT FBX_INCLUDE_DIR)

include_directories(${FBX_INCLUDE_DIR})
link_libraries(${FBX_LIBRARY})
if(APPLE)
  set(ZLIB_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/thirdparty/zlib)
  set(ZLIB_LIBRARY ${CMAKE_SOURCE_DIR}/thirdparty/zlib/lib/zlibstaticd.lib)
  set(PNG_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/thirdparty/libpng)
  set(PNG_LIBRARY ${CMAKE_SOURCE_DIR}/thirdparty/libpng/lib/libpng16_staticd.lib)
  #add_subdirectory(${CMAKE_SOURCE_DIR}/thirdparty/libpng)
endif(APPLE)


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

    SET(LIBRARIES ${PNG_LIBRARIES} Boxer glfw ${GLEW} ${OPENGL} ${CARBON} ${COCOA} ${CORE_VIDEO} ${IOKIT} ${CORE_FOUNDATION} ${FREETYPE} ${BULLET_LIBRARIES} ${PNG_LIBRARY} {$ZLIB_LIBRARY})
endif (APPLE)


######################################################################
# Windows platform specific libs
######################################################################
if (WINDOWS)

    include_directories(${CMAKE_SOURCE_DIR}/thirdparty/glm/)
    find_package(OpenGL REQUIRED)
    find_package(GLEW REQUIRED)
    if (GLEW_FOUND)
        include_directories(${GLEW_INCLUDE_DIRS})
	message(FATAL_ERROR "Glew found at ${GLEW_INCLUDE_DIRS}")
        link_libraries(${GLEW_LIBRARIES})
    endif()
    SET(LIBRARIES Boxer glfw ${GLFW_LIBRARIES} ${OPENGL_LIBRARIES} ${FREETYPE} ${BULLET_LIBRARIES})
endif(WINDOWS)

message(STATUS "Glew found at " ${GLEW_INCLUDE_DIRS})


include_directories(${CMAKE_SOURCE_DIR}/thirdparty/glm/)
find_package(OpenGL REQUIRED)
include_directories( ${OPENGL_INCLUDE_DIRS} )

set(FREETYPE_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/thirdparty/freetype/include)
set(FREETYPE_LIBRARY ${CMAKE_SOURCE_DIR}/thirdparty/freetype/lib/freetyped.lib)
find_package(Freetype REQUIRED)
include_directories(${FREETYPE_INCLUDE_DIRS})

set(BULLET_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/thirdparty/bullet/src)
set(BULLET_DYNAMICS_LIBRARY ${CMAKE_SOURCE_DIR}/thirdparty/bullet/lib/Debug/BulletDynamics_Debug.lib)
set(BULLET_COLLISION_LIBRARY ${CMAKE_SOURCE_DIR}/thirdparty/bullet/lib/Debug/BulletCollision_Debug.lib)
set(BULLET_MATH_LIBRARY  ${CMAKE_SOURCE_DIR}/thirdparty/bullet/lib/Debug/LinearMath_Debug.lib)
set(BULLET_SOFTBODY_LIBRARY ${CMAKE_SOURCE_DIR}/thirdparty/bullet/lib/Debug/BulletSoftBody_Debug.lib)

find_package(Bullet REQUIRED)
include_directories(${BULLET_INCLUDE_DIRS})
set(PNG_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/thirdparty/libpng)
include_directories(${PNG_INCLUDE_DIR})

SET(LIBRARIES glfw ${GLFW_LIBRARIES} ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES}  ${FREETYPE_LIBRARIES} ${BULLET_LIBRARIES} ${PNG_LIBRARY} ${ZLIB_LIBRARY} ${OPENVR_SDK_LIBRARY} ${MONO_LIBRARY})

message(STATUS "Libs:" ${LIBRARIES})


######################################################################
# Define the source files and the executable
######################################################################
set(SOURCE_FILES
# Main sources and headers
AudioSystem.cpp
BoundingBox.cpp
BoundingVolume.cpp
Camera.cpp
Engine.cpp
Entity.cpp
FBXLoader.cpp
GameObject.cpp
Light.cpp
main.cpp
Material.cpp
MD3Loader.cpp
Mesh.cpp
MeshProvider.cpp
Model.cpp
MultipassShader.cpp
Object.cpp
Ocean.cpp
Octree.cpp
OctreeRenderer.cpp
PointLight.cpp
Renderable.cpp
RenderBuffer.cpp
Scene.cpp
Shader.cpp
ShadowMap.cpp
Sky.cpp
SkyRenderer.cpp
SysInfo.cpp
Texture.cpp
Triangle.cpp
utils.cpp
Vertex.cpp
VertexArrayObject.cpp
VertexBufferObject.cpp
WavefrontLoader.cpp
Window.cpp
AnimationFrame.h
AudioSystem.h
BoundingBox.h
BoundingVolume.h
Camera.h
Engine.h
Entity.h
FBXLoader.h
GameObject.h
GL_error.h
Light.h
Material.h
MathUtils.h
MD3Loader.h
Mesh.h
MeshProvider.h
Model.h
MultipassShader.h
Object.h
Ocean.h
Octree.h
OctreeRenderer.h
Omen_OpenGL.h
PointLight.h
Renderable.h
RenderBuffer.h
Scene.h
Shader.h
ShadowMap.h
Signal.h
Sky.h
SkyRenderer.h
TextRenderer.h
Texture.h
Triangle.h
utils.h
Vertex.h
VertexArrayObject.h
VertexBufferObject.h
WavefrontLoader.h
Window.h
SysInfo.h
Property.cpp
Signal.cpp
Ocean.cpp
FFT.cpp
Skybox.cpp
SkyboxRenderer.cpp
StringTools.cpp

#/component/
component/BorderSprite.cpp
component/CameraController.cpp
component/Clickable.cpp
component/Component.cpp
component/Draggable.cpp
component/JoystickInput.cpp
component/KeyboardInput.cpp
component/MeshAnimator.cpp
component/MeshController.cpp
component/MeshRenderer.cpp
component/MouseInput.cpp
component/Picker.cpp
component/Renderer.cpp
component/Script.cpp
component/Sprite.cpp
component/SpriteRenderer.cpp
component/TextRenderer.cpp
component/Transform.cpp
component/Transformer.cpp
component/BorderSprite.h
component/CameraController.h
component/Clickable.h
component/Component.h
component/Draggable.h
component/JoystickInput.h
component/KeyboardInput.h
component/MeshAnimator.h
component/MeshController.h
component/MeshRenderer.h
component/MouseInput.h
component/Picker.h
component/Renderer.h
component/Script.h
component/Sprite.h
component/SpriteRenderer.h
component/TextRenderer.h
component/Transform.h
component/Transformer.h

# /system/
system/CoreSystem.cpp
system/GraphicsSystem.cpp
system/InputSystem.cpp
system/PhysicsSystem.cpp
system/System.cpp
system/CoreSystem.h
system/GraphicsSystem.h
system/InputSystem.h
system/PhysicsSystem.h
system/System.h
system/OpenVRSystem.cpp
system/OpenVRSystem.h

# /ui/
ui/Button.cpp
ui/FrameLayout.cpp
ui/ImageView.cpp
ui/Layout.cpp
ui/LinearLayout.cpp
ui/Slider.cpp
ui/TextView.cpp
ui/View.cpp
ui/Button.h
ui/FrameLayout.h
ui/ImageView.h
ui/Layout.h
ui/LinearLayout.h
ui/Slider.h
ui/TextView.h
ui/View.h

# thirdparty
thirdparty/boxer/lib/boxer_win.cpp
Common/Common.cxx
Common/Common.h
)
add_executable(Omen ${SOURCE_FILES})
#SET_TARGET_PROPERTIES(Omen PROPERTIES LINKER_LANGUAGE C++)

set_property(TARGET Omen PROPERTY CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std:c++latest")

######################################################################
# Set the target link libraries
######################################################################
target_link_libraries(Omen ${LIBRARIES})

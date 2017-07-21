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
SET(MSVC_INCREMENTAL_DEFAULT ON)
project(Omen)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)
set(CMAKE_CXX_STANDARsD 17)
set(CMAKE_BUILD_TYPE Debug)
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


set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /MDd ")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj /MDd -std:c++latest /std:c++latest ")

if(MSVC)
# We statically link to reduce dependencies
foreach(flag_var CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
    if(${flag_var} MATCHES "/MD")
        string(REGEX REPLACE "/MD" "/MDd" ${flag_var} "${${flag_var}}")
    endif(${flag_var} MATCHES "/MD")
	if(${flag_var} MATCHES "/MDdd")
        string(REGEX REPLACE "/MDdd" "/MDd" ${flag_var} "${${flag_var}}")
    endif(${flag_var} MATCHES "/MDdd")
	if(${flag_var} MATCHES "/MT")
        string(REGEX REPLACE "/MT" "/MDd" ${flag_var} "${${flag_var}}")
    endif(${flag_var} MATCHES "/MT")
    #if(${flag_var} MATCHES "/MDd")
    #    string(REGEX REPLACE "/MDd" "/MTd" ${flag_var} "${${flag_var}}")
    #endif(${flag_var} MATCHES "/MDd")
endforeach(flag_var)
endif(MSVC)


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

if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj /MDd")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")
endif()

######################################################################
# GLEW - OpenGL extension wrangler library
######################################################################
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
add_subdirectory(${CMAKE_SOURCE_DIR}/thirdparty/glew/build/cmake)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	link_libraries(${CMAKE_BUILD_DIR}lib/${CMAKE_BUILD_TYPE}/glew32d)
else()
	link_libraries(${CMAKE_BUILD_DIR}lib/${CMAKE_BUILD_TYPE}/glew32)
endif()

######################################################################
# Google Test library (WIP)
######################################################################
#add_subdirectory(${CMAKE_SOURCE_DIR}/thirdparty/googletest)

######################################################################
# FREETYPE
######################################################################
set(FREETYPE_NO_DIST TRUE)
add_subdirectory(${CMAKE_SOURCE_DIR}/thirdparty/freetype)
include_directories(${CMAKE_SOURCE_DIR}/thirdparty/freetype/include)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	link_libraries(${CMAKE_BUILD_DIR}lib/${CMAKE_BUILD_TYPE}/freetyped)
else()
	link_libraries(${CMAKE_BUILD_DIR}lib/${CMAKE_BUILD_TYPE}/freetype)
endif()

######################################################################
# OpenVR SDK
######################################################################
include_directories(${CMAKE_SOURCE_DIR}/thirdparty/openvr/headers)
set(OPENVR_SDK_LIBRARY ${CMAKE_SOURCE_DIR}/thirdparty/openvr/lib/win32/openvr_api.lib)
# add_custom_command(TARGET Omen POST_BUILD        # Adds a post-build event to MyTest
#   COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
#        "${PROJECT_SOURCE_DIR}/thirdparty/openvr/bin/win32/*"      # <--this is in-file
#        $<TARGET_FILE_DIR:Omen>)                 # <--this is out-file path

######################################################################
# OpenAL
######################################################################
#include_directories(${CMAKE_SOURCE_DIR}/thirdparty/openal-soft/include)
#set(OPENAL_LIBRARY ${CMAKE_SOURCE_DIR}/thirdparty/openal-soft/lib/win32/openvr_api.lib)
#add_subdirectory(${CMAKE_SOURCE_DIR}/thirdparty/openal-soft)

######################################################################
# Mono SDK
######################################################################
include_directories(${CMAKE_SOURCE_DIR}/thirdparty/mono/msvc/include)
include_directories(${CMAKE_SOURCE_DIR}/thirdparty/mono/include/mono-2.0)
set(MONO_LIBRARY ${CMAKE_SOURCE_DIR}/thirdparty/mono/msvc/build/sgen/Win32/lib/${CMAKE_BUILD_TYPE}/mono-2.0-sgen.lib)

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
# ZLIB
######################################################################
add_subdirectory(${CMAKE_SOURCE_DIR}/thirdparty/zlib)
include_directories(${ZLIB_INCLUDE_DIRS})
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	link_libraries(${CMAKE_BUILD_DIR}lib/${CMAKE_BUILD_TYPE}/zlibd)
else()
	link_libraries(${CMAKE_BUILD_DIR}lib/${CMAKE_BUILD_TYPE}/zlib)
endif()


######################################################################
# LIBPNG
######################################################################
set(ZLIB_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/thirdparty/zlib)
set(ZLIB_LIBRARY_DEBUG ${CMAKE_SOURCE_DIR}/build/lib/Debug/zlibd.lib)
set(ZLIB_LIBRARY_RELEASE ${CMAKE_SOURCE_DIR}/build/lib/Release/zlib.lib)
#if (APPLE)
#  find_package(Png REQUIRED)
#  if(NOT PNG_FOUND)
#      message(FATAL_ERROR "Png not found")
#  endif (NOT PNG_FOUND)
#else(APPLE)
 # set(ZLIB_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/thirdparty/zlib)
 # set(ZLIB_LIBRARY ${CMAKE_SOURCE_DIR}/thirdparty/zlib/lib/zlibstaticd.lib)
 # set(PNG_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/thirdparty/libpng)
 # set(PNG_LIBRARY ${CMAKE_SOURCE_DIR}/thirdparty/libpng/lib/libpng16_staticd.lib)
 # #add_subdirectory(${CMAKE_SOURCE_DIR}/thirdparty/libpng)
#endif(APPLE)
set(PNG_DEBUG ON)
add_subdirectory(${CMAKE_SOURCE_DIR}/thirdparty/libpng)
set(PNG_LIBRARY ${CMAKE_SOURCE_DIR}/build/lib/Debug/libpng16d.lib)

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

#set(FREETYPE_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/thirdparty/freetype/include)
#set(FREETYPE_LIBRARY ${CMAKE_SOURCE_DIR}/thirdparty/freetype/lib/freetyped.lib)
#find_package(Freetype REQUIRED)
#include_directories(${FREETYPE_INCLUDE_DIRS})

set(BULLET_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/thirdparty/bullet/src)
set(BULLET_DYNAMICS_LIBRARY ${CMAKE_SOURCE_DIR}/build/lib/${CMAKE_BUILD_TYPE}/BulletDynamics.lib)
set(BULLET_COLLISION_LIBRARY ${CMAKE_SOURCE_DIR}/build/lib/${CMAKE_BUILD_TYPE}/BulletCollision.lib)
set(BULLET_MATH_LIBRARY  ${CMAKE_SOURCE_DIR}/build/lib/${CMAKE_BUILD_TYPE}/LinearMath.lib)
set(BULLET_SOFTBODY_LIBRARY ${CMAKE_SOURCE_DIR}/build/lib/${CMAKE_BUILD_TYPE}/BulletSoftBody.lib)

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
Editor/EditorWindow.cpp
Editor/EditorCameraView.cpp
Editor/GUILayout.cpp
Editor/EditorScene.cpp
component/EditorSceneRenderer.cpp
Editor/WindowDivider.cpp
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
## Headers
Editor/EditorWindow.h
Editor/EditorCameraview.h
Editor/GUILayout.h
Editor/EditorScene.h
component/EditorSceneRenderer.h
Editor/WindowDivider.h
component/PlainColorRenderer.h
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
component/PlainColorRenderer.cpp
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
thirdparty/boxer/src/boxer_win.cpp
Common/Common.cxx
Common/Common.h
)
add_executable(Omen ${SOURCE_FILES})
#SET_TARGET_PROPERTIES(Omen PROPERTIES LINKER_LANGUAGE C++)
set_property(TARGET Omen PROPERTY CXX_STANDARD 17)

######################################################################
# Set the target link libraries
######################################################################
target_link_libraries(Omen ${LIBRARIES})
set_target_properties(Omen PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")

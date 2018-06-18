######################################################################
# omen Game engine project
#
# Lauri Kortevaara (2015-2018)
# lauri.kortevaara@gmail.com
#
# https://github.com/laurikortevaara/omen
#
######################################################################
cmake_minimum_required(VERSION 3.9.6)
set(MSVC_INCREMENTAL_DEFAULT ON)
set(CMAKE_CONFIGURATION_TYPES "Debug;Release;RelWithDebInfo" CACHE PATH "Configuration types")

project(Omen)

include(ExternalProject)

set(CMAKE_BUILD_DIR ${CMAKE_SOURCE_DIR}/build/)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)

option(BUILD_MONO_DEVELOP "Build Mono Develop SDK" OFF)
option(BUILD_BULLET_PHYSICS "Build Bullet3 Physics library" OFF)
option(BUILD_OPENAL "Build OpenAL Soft 3D Audio API" OFF)
option(BUILD_OPENVR "Build OpenVR SDK" OFF)

######################################################################
# set the working directory for debugging
######################################################################
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

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
system/

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
Common/Common.cxx
Common/Common.h
)
add_executable(Omen ${SOURCE_FILES})

######################################################################
# C++17
######################################################################
set_target_properties(Omen PROPERTIES
                      VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
                      CXX_STANDARD 17
                      )

# Disable warning regarding utf8 conversions warning 
# warning C4996 : 'std::codecvt_utf8_utf16<wchar_t,1114111,0>' : warning STL4017 : 
# std::wbuffer_convert, std::wstring_convert, and the <codecvt> header(
#    containing std::codecvt_mode, std::codecvt_utf8, std::codecvt_utf16, 
#    and std::codecvt_utf8_utf16) are deprecated in C++17. 
#    (The std::codecvt class template is NOT deprecated.) 
#    The C++ Standard doesn't provide equivalent non-deprecated functionality; 
#    consider using MultiByteToWideChar() and WideCharToMultiByte() from <Windows.h> instead. 
#    You can define 
#    _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING or 
#    _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS to acknowledge that you have received this warning.
add_definitions(-D_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING)
add_definitions(-D_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS)
set(CMAKE_BUILD_TYPE Release)
add_definitions(-DCMAKE_BUILD_TYPE=Release)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj /MDd")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")

######################################################################
# OPENGL
######################################################################
find_package(OpenGL REQUIRED)
target_link_libraries(Omen ${OPENGL_LIBRARIES})

if(NOT EXISTS "${CMAKE_SOURCE_DIR}/thirdparty/opengl/include/GL/glext.h")	
	message("Downloading: https://www.khronos.org/registry/OpenGL/api/GL/glext.h" )
	file(DOWNLOAD https://www.khronos.org/registry/OpenGL/api/GL/glext.h ${CMAKE_SOURCE_DIR}/thirdparty/opengl/include/GL/glext.h SHOW_PROGRESS)
else()
	message("GL/glext.h found" )
endif()
if(NOT EXISTS "${CMAKE_SOURCE_DIR}/thirdparty/opengl/include/GL/glcorearb.h")
	message("Downloading: https://www.khronos.org/registry/OpenGL/api/GL/glcorearb.h" )
	file(DOWNLOAD https://www.khronos.org/registry/OpenGL/api/GL/glcorearb.h ${CMAKE_SOURCE_DIR}/thirdparty/opengl/include/GL/glcorearb.h SHOW_PROGRESS)
else()
	message("GL/glcorearb.h found" )
endif()

if(NOT EXISTS "${CMAKE_SOURCE_DIR}/thirdparty/opengl/include/GL/glxext.h")
	message("Downloading: https://www.khronos.org/registry/OpenGL/api/GL/glxext.h" )
	file(DOWNLOAD https://www.khronos.org/registry/OpenGL/api/GL/glxext.h ${CMAKE_SOURCE_DIR}/thirdparty/opengl/include/GL/glxext.h SHOW_PROGRESS)
else()
	message("GL/glxext.h found" )	
endif()
if(NOT EXISTS "${CMAKE_SOURCE_DIR}/thirdparty/opengl/include/GL/wglext.h")
	message("Downloading:https://www.khronos.org/registry/OpenGL/api/GL/wglext.h" )
	file(DOWNLOAD https://www.khronos.org/registry/OpenGL/api/GL/wglext.h ${CMAKE_SOURCE_DIR}/thirdparty/opengl/include/GL/wglext.h SHOW_PROGRESS)
else()
	message("GL/wglext.h found" )	
endif()
if(NOT EXISTS "${CMAKE_SOURCE_DIR}/thirdparty/opengl/include/KHR/khrplatform.h")
	message("Downloading: https://www.khronos.org/registry/EGL/api/KHR/khrplatform.h" )
	file(DOWNLOAD https://www.khronos.org/registry/EGL/api/KHR/khrplatform.h ${CMAKE_SOURCE_DIR}/thirdparty/opengl/include/KHR/khrplatform.h SHOW_PROGRESS)
else()
	message("KHR/khrplatform.h found" )
endif()

include_directories( ${CMAKE_SOURCE_DIR}/thirdparty/opengl/include)

######################################################################
# GLEW
######################################################################
add_definitions(-DGLEW_STATIC)
add_subdirectory(${CMAKE_SOURCE_DIR}/thirdparty/glew)
add_dependencies(Omen glew)
target_link_libraries(Omen ${GLEW_LIBRARY_STATIC})
message( "Glew include dir: " ${GLEW_INCLUDE_DIR})
message( "Glew library: " ${GLEW_LIBRARY_STATIC})
set(GLEW_INCLUDE_DIR ${GLEW_INCLUDE_DIR})
set(GLEW_LIBRARY ${GLEW_LIBRARY_STATIC})
link_directories(${GLEW_ROOT}/src/glew/lib/Release/Win32)
find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIR})

######################################################################
# GLFW
######################################################################
add_subdirectory(${CMAKE_SOURCE_DIR}/thirdparty/glfw)
add_dependencies(Omen glfw)
target_link_libraries(Omen ${GLFW_LIBRARY_STATIC})

######################################################################
# glm
######################################################################
add_subdirectory(${CMAKE_SOURCE_DIR}/thirdparty/glm)
add_dependencies(Omen glm)

######################################################################
# zlib
######################################################################
add_subdirectory(${CMAKE_SOURCE_DIR}/thirdparty/zlib)
add_dependencies(Omen zlib)
include_directories(${ZLIB_INCLUDE_DIR})
target_link_libraries(Omen ${ZLIB_LIBRARY_STATIC})

######################################################################
# libpng
######################################################################
add_subdirectory(${CMAKE_SOURCE_DIR}/thirdparty/libpng)
add_dependencies(Omen libpng)
include_directories(${LIBPNG_INCLUDE_DIR})
target_link_libraries(Omen ${LIBPNG_LIBRARY_STATIC})

######################################################################
# Fbx SDK
######################################################################
add_definitions(-DFBXSDK_SHARED)
add_subdirectory(${CMAKE_SOURCE_DIR}/thirdparty/fbxsdk)
include_directories(${FBX_INCLUDE_DIR})
target_link_libraries(Omen optimized ${FBX_LIBRARY_RELEASE} debug ${FBX_LIBRARY_RELEASE})
add_custom_target(FbxSdkCopyBinaries
  COMMAND ${CMAKE_COMMAND} -E copy ${FBX_DLL} ${CMAKE_SOURCE_DIR}/bin
  )
add_dependencies(Omen FbxSdkCopyBinaries)

######################################################################
# tinydir
######################################################################
add_subdirectory(${CMAKE_SOURCE_DIR}/thirdparty/tinydir)
add_dependencies(Omen tinydir)

######################################################################
# STB
######################################################################
add_subdirectory(${CMAKE_SOURCE_DIR}/thirdparty/stb)
add_dependencies(Omen stb)

######################################################################
# Boxer
######################################################################
add_subdirectory(${CMAKE_SOURCE_DIR}/thirdparty/boxer)
add_dependencies(Omen boxer)
target_link_libraries(Omen ${BOXER_LIBRARY_STATIC})

######################################################################
# Freetype
######################################################################
add_subdirectory(${CMAKE_SOURCE_DIR}/thirdparty/freetype)
add_dependencies(Omen freetype)
target_link_libraries(Omen ${FREETYPE_LIBRARY_STATIC})

######################################################################
# OpenVR SDK
######################################################################
if(BUILD_OPENVR)
	add_subdirectory(${CMAKE_SOURCE_DIR}/thirdparty/openvr)
	add_dependencies(Omen openvr)
endif()

######################################################################
# Mono (C#) runtime
######################################################################
if(BUILD_MONO_DEVELOP)
	add_subdirectory(${CMAKE_SOURCE_DIR}/thirdparty/mono)
	add_dependencies(Omen mono)
endif()

######################################################################
# Bullet Physics
######################################################################
if(BUILD_BULLET_PHYSICS)
	add_subdirectory(${CMAKE_SOURCE_DIR}/thirdparty/bullet)
	add_dependencies(Omen bullet)
endif()

######################################################################
# OpenAL Audio library 
######################################################################
if(BUILD_OPENAL)
	add_subdirectory(${CMAKE_SOURCE_DIR}/thirdparty/openal)
	add_dependencies(Omen openal)
endif()
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

project(OmenEngine)
set( OMEN_LIBRARY_NAME "OmenEngine" )

# Setup build options
option(BUILD_SHARED "Builds the library as shared library" OFF)
option(BUILD_FRAMEWORK "Builds the library as an apple Framework" OFF)
option(BUILD_UNIVERSAL "Builds the shared or framework as a universal (fat, 32- & 64-bit) binary" ON)
option(BUILD_OSX_I386 "Builds the shared or framework as a 32-bit binary, even on a 64-bit platform" OFF)
option(USE_LIBCXX "Uses libc++ instead of libstdc++" ON)
option(USE_CUSTOM_LIBCXX "Uses a custom libc++" OFF)
set(MSVC_INCREMENTAL_DEFAULT ON)
set(CMAKE_CONFIGURATION_TYPES "Debug;Release;RelWithDebInfo" CACHE PATH "Configuration types")


# Check if 32 or 64 bit system.
set(SIZEOF_VOIDP ${CMAKE_SIZEOF_VOID_P})
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  set(PROCESSOR_ARCH "64")
else()
  set(PROCESSOR_ARCH "32")
endif()

# Get platform.
if(WIN32)
	set(PLATFORM_NAME "win")
elseif(UNIX AND NOT APPLE)
  if(CMAKE_SYSTEM_NAME MATCHES ".*Linux")
    set(PLATFORM_NAME "linux")
    add_definitions(-DLINUX -DPOSIX)
    if(PROCESSOR_ARCH MATCHES "64")
        add_definitions(-DLINUX64)
    endif()
  endif()
elseif(APPLE)
  if(CMAKE_SYSTEM_NAME MATCHES ".*Darwin.*" OR CMAKE_SYSTEM_NAME MATCHES ".*MacOS.*")
    set(PLATFORM_NAME "osx")
    add_definitions(-DOSX -DPOSIX)
    if(BUILD_UNIVERSAL)
      set(CMAKE_OSX_ARCHITECTURES "i386;x86_64")
    endif()
    if(BUILD_OSX_I386)
      set(PROCESSOR_ARCH "32")
      set(CMAKE_OSX_ARCHITECTURES "i386")
    endif()
  endif()
endif()

# Set output folder for static and shared libraries
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${PLATFORM_NAME}${PROCESSOR_ARCH})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${PLATFORM_NAME}${PROCESSOR_ARCH})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${PLATFORM_NAME}${PROCESSOR_ARCH})

include(ExternalProject)

set(CMAKE_BUILD_DIR ${CMAKE_SOURCE_DIR}/build/)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMake/modules)

option(BUILD_MONO_DEVELOP "Build Mono Develop SDK" OFF)
option(BUILD_BULLET_PHYSICS "Build Bullet3 Physics library" OFF)
option(BUILD_OPENAL "Build OpenAL Soft 3D Audio API" OFF)
option(BUILD_OPENVR "Build OpenVR SDK" OFF)

# Include source files under src
add_subdirectory(src)
# Include header files under include
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

option(BUILD_SHARED "Builds the library as shared library" OFF)
option(BUILD_FRAMEWORK "Builds the library as an apple Framework" OFF)

if(BUILD_SHARED)
	add_library(${OMEN_LIBRARY_NAME} SHARED ${SOURCE_FILES})
elseif(BUILD_FRAMEWORK)
	set( OMEN_LIBRARY_NAME "Omen" )
	add_library( ${OMEN_LIBRARY_NAME} 
            SHARED ${SOURCE_FILES}
            #${CMAKE_SOURCE_DIR}/headers/omen.h
	)
	set_target_properties(Omen PROPERTIES
		FRAMEWORK TRUE
		FRAMEWORK_VERSION A
		MACOSX_FRAMEWORK_IDENTIFIER com.laurikortevaara.Omen.framework
		MACOSX_FRAMEWORK_INFO_PLIST ${CMAKE_SOURCE_DIR}/src/Info.plist
		# "current version" in semantic format in Mach-O binary file
		VERSION 1.0.0
		# "compatibility version" in semantic format in Mach-O binary file
		SOVERSION 1.0.0
		PUBLIC_HEADER "${CMAKE_SOURCE_DIR}/headers/omen.h;"
		LINKER_LANGUAGE CXX
	)
else()
	add_library(${OMEN_LIBRARY_NAME}Static STATIC ${SOURCE_FILES})
	add_library(${OMEN_LIBRARY_NAME}Shared SHARED ${SOURCE_FILES})
endif()

if(USE_CUSTOM_LIBCXX)
	set(EXTRA_LIBS ${EXTRA_LIBS} c++ c++abi)
endif()

#target_link_libraries(${OMEN_LIBRARY_NAME} ${EXTRA_LIBS})

install(TARGETS ${OMEN_LIBRARY_NAME}Static DESTINATION lib)
install(TARGETS ${OMEN_LIBRARY_NAME}Shared DESTINATION lib)


######################################################################
# C++17
######################################################################
set_target_properties(${OMEN_LIBRARY_NAME}Static PROPERTIES
                      VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
                      CXX_STANDARD 17
                      )
set_target_properties(${OMEN_LIBRARY_NAME}Shared PROPERTIES
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
target_link_libraries(${OMEN_LIBRARY_NAME}Static ${OPENGL_LIBRARIES})
target_link_libraries(${OMEN_LIBRARY_NAME}Shared ${OPENGL_LIBRARIES})

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

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/opengl/include)

######################################################################
# GLEW
######################################################################
set(DEPENDENCY "glew")
add_subdirectory(thirdparty/${DEPENDENCY})

add_dependencies(${OMEN_LIBRARY_NAME}Static ${DEPENDENCY})
add_dependencies(${OMEN_LIBRARY_NAME}Shared ${DEPENDENCY})

target_link_libraries(${OMEN_LIBRARY_NAME}Static ${GLEW_LIBRARY_STATIC_WIN32})
target_link_libraries(${OMEN_LIBRARY_NAME}Shared ${GLEW_LIBRARY_SHARED_WIN32})

set(GLEW_INCLUDE_DIR ${GLEW_INCLUDE_DIR})
set(GLEW_LIBRARY ${GLEW_LIBRARY_STATIC_WIN32})
link_directories(${GLEW_ROOT}/src/glew/lib/Release/Win32)
find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIR})

######################################################################
# GLFW
######################################################################
set(DEPENDENCY "glfw")
add_subdirectory(thirdparty/${DEPENDENCY})
add_dependencies(${OMEN_LIBRARY_NAME}Static ${DEPENDENCY})
add_dependencies(${OMEN_LIBRARY_NAME}Shared ${DEPENDENCY})
target_link_libraries(${OMEN_LIBRARY_NAME}Static ${GLFW_LIBRARY_STATIC})
target_link_libraries(${OMEN_LIBRARY_NAME}Shared ${GLFW_LIBRARY_STATIC})
include_directories(${GLFW_INCLUDE_DIR})

######################################################################
# glm
######################################################################
set(DEPENDENCY "glm")
add_subdirectory(thirdparty/${DEPENDENCY})
add_dependencies(${OMEN_LIBRARY_NAME}Static ${DEPENDENCY})
add_dependencies(${OMEN_LIBRARY_NAME}Shared ${DEPENDENCY})
include_directories(${GLM_INCLUDE_DIR})

######################################################################
# zlib
######################################################################
set(DEPENDENCY "zlib")
add_subdirectory(thirdparty/${DEPENDENCY})
add_dependencies(${OMEN_LIBRARY_NAME}Static ${DEPENDENCY})
add_dependencies(${OMEN_LIBRARY_NAME}Shared ${DEPENDENCY})
target_link_libraries(${OMEN_LIBRARY_NAME}Static ${ZLIB_LIBRARY_STATIC})
target_link_libraries(${OMEN_LIBRARY_NAME}Shared ${ZLIB_LIBRARY_STATIC})
include_directories(${ZLIB_INCLUDE_DIR})

######################################################################
# libpng
######################################################################
set(DEPENDENCY "libpng")
add_subdirectory(thirdparty/${DEPENDENCY})
add_dependencies(${OMEN_LIBRARY_NAME}Static ${DEPENDENCY})
add_dependencies(${OMEN_LIBRARY_NAME}Shared ${DEPENDENCY})
target_link_libraries(${OMEN_LIBRARY_NAME}Static ${LIBPNG_LIBRARY_STATIC_WIN32})
target_link_libraries(${OMEN_LIBRARY_NAME}Shared ${LIBPNG_LIBRARY_STATIC_WIN32})
include_directories(${LIBPNG_INCLUDE_DIR})

######################################################################
# Fbx SDK
######################################################################
add_definitions(-DFBXSDK_SHARED)
set(DEPENDENCY "fbxsdk")
add_subdirectory(thirdparty/${DEPENDENCY})
target_link_libraries(${OMEN_LIBRARY_NAME}Static optimized ${FBX_LIBRARY} debug ${FBX_LIBRARY_DEBUG})
target_link_libraries(${OMEN_LIBRARY_NAME}Shared optimized ${FBX_LIBRARY} debug ${FBX_LIBRARY_DEBUG})
add_custom_target(FbxSdkCopyBinaries
  COMMAND ${CMAKE_COMMAND} -E copy ${FBX_DLL} ${CMAKE_SOURCE_DIR}/bin
  )
add_dependencies(${OMEN_LIBRARY_NAME}Static FbxSdkCopyBinaries)
add_dependencies(${OMEN_LIBRARY_NAME}Shared FbxSdkCopyBinaries)
include_directories(${FBX_INCLUDE_DIR})

######################################################################
# tinydir
######################################################################
set(DEPENDENCY "tinydir")
add_subdirectory(thirdparty/${DEPENDENCY})
add_dependencies(${OMEN_LIBRARY_NAME}Static ${DEPENDENCY})
add_dependencies(${OMEN_LIBRARY_NAME}Shared ${DEPENDENCY})
include_directories(${TINYDIR_INCLUDE_DIR})

######################################################################
# STB
######################################################################
set(DEPENDENCY "stb")
add_subdirectory(thirdparty/${DEPENDENCY})
add_dependencies(${OMEN_LIBRARY_NAME}Static ${DEPENDENCY})
add_dependencies(${OMEN_LIBRARY_NAME}Shared ${DEPENDENCY})
include_directories(${STB_INCLUDE_DIR})

######################################################################
# Boxer
######################################################################
set(DEPENDENCY "boxer")
add_subdirectory(thirdparty/${DEPENDENCY})
add_dependencies(${OMEN_LIBRARY_NAME}Static ${DEPENDENCY})
add_dependencies(${OMEN_LIBRARY_NAME}Shared ${DEPENDENCY})
target_link_libraries(${OMEN_LIBRARY_NAME}Static ${BOXER_LIBRARY_STATIC})
target_link_libraries(${OMEN_LIBRARY_NAME}Shared ${BOXER_LIBRARY_STATIC})
include_directories(${BOXER_INCLUDE_DIR})

######################################################################
# Freetype
######################################################################
set(DEPENDENCY "freetype")
add_subdirectory(thirdparty/${DEPENDENCY})
add_dependencies(${OMEN_LIBRARY_NAME}Static ${DEPENDENCY})
add_dependencies(${OMEN_LIBRARY_NAME}Shared ${DEPENDENCY})
target_link_libraries(${OMEN_LIBRARY_NAME}Static ${FREETYPE_LIBRARY_STATIC})
target_link_libraries(${OMEN_LIBRARY_NAME}Shared ${FREETYPE_LIBRARY_STATIC})
include_directories(${FREETYPE_INCLUDE_DIR})

######################################################################
# OpenVR SDK
######################################################################
if(BUILD_OPENVR)
	set(DEPENDENCY "openvr")
	add_subdirectory(thirdparty/${DEPENDENCY})
	add_dependencies(${OMEN_LIBRARY_NAME}Static ${DEPENDENCY})
	add_dependencies(${OMEN_LIBRARY_NAME}Shared ${DEPENDENCY})
endif()

######################################################################
# Mono (C#) runtime
######################################################################
if(BUILD_MONO_DEVELOP)
	set(DEPENDENCY "mono")
	add_subdirectory(thirdparty/${DEPENDENCY})
	add_dependencies(${OMEN_LIBRARY_NAME}Static ${DEPENDENCY})
	add_dependencies(${OMEN_LIBRARY_NAME}Shared ${DEPENDENCY})
endif()

######################################################################
# Bullet Physics
######################################################################
if(BUILD_BULLET_PHYSICS)
	set(DEPENDENCY "bullet")
	add_subdirectory(thirdparty/${DEPENDENCY})
	add_dependencies(${OMEN_LIBRARY_NAME}Static ${DEPENDENCY})
	add_dependencies(${OMEN_LIBRARY_NAME}Shared ${DEPENDENCY})
endif()

######################################################################
# OpenAL Audio library 
######################################################################
if(BUILD_OPENAL)
	set(DEPENDENCY "openal")
	add_subdirectory(thirdparty/${DEPENDENCY})
	add_dependencies(${OMEN_LIBRARY_NAME}Static ${DEPENDENCY})
	add_dependencies(${OMEN_LIBRARY_NAME}Shared ${DEPENDENCY})
endif()

# Define include directory for library users
set(OMEN_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/include;${GLM_INCLUDE_DIR};" PARENT_SCOPE)

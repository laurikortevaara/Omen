# Locate FBX
# This module defines:
# FBX_INCLUDE_DIR, where to find the headers
#
# FBX_LIBRARY, FBX_LIBRARY_DEBUG
# FBX_FOUND
#
# $FBX_DIR is an environment variable that would
# correspond to the ./configure --prefix=$FBX_DIR

message(STATUS "Finding FBX SDK")

IF(APPLE)
    SET(FBX_LIBDIR "gcc4/ub")
ELSEIF(CMAKE_COMPILER_IS_GNUCXX)
    SET(FBX_LIBDIR "gcc4")
ELSE()
    SET(FBX_LIBDIR "vs2015")
ENDIF()

IF(APPLE)
    # do nothing
ELSEIF(CMAKE_CL_64)
    SET(FBX_LIBDIR ${FBX_LIBDIR}/x64)
ELSEIF(CMAKE_COMPILER_IS_GNUCXX AND CMAKE_SIZEOF_VOID_P EQUAL 8)
    SET(FBX_LIBDIR ${FBX_LIBDIR}/x64)
ELSE()
    SET(FBX_LIBDIR ${FBX_LIBDIR}/x86)
ENDIF()

#try to use 2019.0

IF(APPLE)
    SET(FBX_LIBNAME "libfbxsdk")
ELSEIF(CMAKE_COMPILER_IS_GNUCXX)
    SET(FBX_LIBNAME "fbxsdk")
ELSE()
    SET(FBX_LIBNAME "libfbxsdk")
ENDIF()

SET(FBX_LIBNAME_DEBUG ${FBX_LIBNAME}d)

message(STATUS "FBX LibDir: " ${FBX_LIBDIR})
message(STATUS "FBX LibName: " ${FBX_LIBNAME})

SET( FBX_SEARCH_PATHS
    "$ENV{ProgramW6432}/Autodesk/FBX/FBX SDK/2019.0"
)

message(STATUS "FBX SearchPaths: " ${FBX_SEARCH_PATHS})

# search for headers & debug/release libraries
FIND_PATH(FBX_INCLUDE_DIR "fbxsdk.h"
    PATHS ${FBX_SEARCH_PATHS}
    PATH_SUFFIXES "include")

include_directories(${FBX_INCLUDE_DIR})
	
SET(CMAKE_FIND_LIBRARY_SUFFIXES ".lib")
FIND_LIBRARY( FBX_LIBRARY libfbxsdk.lib
    PATHS ${FBX_SEARCH_PATHS}
    PATH_SUFFIXES "lib/${FBX_LIBDIR}/release" "lib/${FBX_LIBDIR}")
	
FIND_LIBRARY( FBX_LIBRARY_DEBUG libfbxsdk.lib
    PATHS ${FBX_SEARCH_PATHS}
    PATH_SUFFIXES "lib/${FBX_LIBDIR}/debug" "lib/${FBX_LIBDIR}")

message(STATUS "FBX IncludeDir: " ${FBX_INCLUDE_DIR})
message(STATUS "FBX Library(Releaes): " ${FBX_LIBRARY})
message(STATUS "FBX Library(Debug): " ${FBX_LIBRARY_DEBUG})

#Once one of the calls succeeds the result variable will be set and stored in the cache so that no call will search again.

#no debug d suffix, search in debug folder only
FIND_LIBRARY( FBX_LIBRARY_DEBUG_SHARED NAMES ${FBX_LIBNAME} ${FBX_LIBNAME_DEBUG}
    PATHS ${FBX_SEARCH_PATHS}
    PATH_SUFFIXES "lib/${FBX_LIBDIR}/debug" "lib/${FBX_LIBDIR}")

FIND_LIBRARY( FBX_LIBRARY_RELEASE_SHARED ${FBX_LIBNAME}
    PATHS ${FBX_SEARCH_PATHS}
    PATH_SUFFIXES "lib/${FBX_LIBDIR}/release" "lib/${FBX_LIBDIR}")

message(STATUS "FBX Library Release: " ${FBX_LIBRARY_RELEASE_SHARED})
message(STATUS "FBX Library Debug: " ${FBX_LIBRARY_DEBUG_SHARED})

# Find Dll
#SET(CMAKE_FIND_LIBRARY_PREFIXES "")
SET(CMAKE_FIND_LIBRARY_SUFFIXES ".dll")
FIND_LIBRARY( FBX_DLL ${FBX_LIBNAME}
    PATHS ${FBX_SEARCH_PATHS}
    PATH_SUFFIXES "lib/${FBX_LIBDIR}/release" "lib/${FBX_LIBDIR}")
IF(FBX_DLL)
	message(STATUS "FBX DLL: " ${FBX_DLL})
ELSE()
	MESSAGE("Did not find FBX DLL")
ENDIF()

SET(CMAKE_FIND_LIBRARY_SUFFIXES ".lib")

IF(FBX_LIBRARY AND FBX_LIBRARY_DEBUG AND FBX_INCLUDE_DIR)
    SET(FBX_FOUND "YES")
ELSE()
    SET(FBX_FOUND "NO")
ENDIF()
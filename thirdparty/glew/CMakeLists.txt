project(glew)

######################################################################
# glew
######################################################################
set(GLEW_ROOT ${CMAKE_BINARY_DIR}/thirdparty/${PROJECT_NAME})
set(GLEW_ROOT ${CMAKE_BINARY_DIR}/thirdparty/${PROJECT_NAME} PARENT_SCOPE)
set(GLEW_INCLUDE_DIR ${GLEW_ROOT}/src/glew/include)
set(GLEW_INCLUDE_DIR ${GLEW_ROOT}/src/glew/include PARENT_SCOPE)
set(GLEW_LIBRARY_STATIC ${CMAKE_BINARY_DIR}/thirdparty/${PROJECT_NAME}/src/glew/lib/Release/Win32/glew32s.lib)
set(GLEW_LIBRARY_STATIC ${CMAKE_BINARY_DIR}/thirdparty/${PROJECT_NAME}/src/glew/lib/Release/Win32/glew32s.lib PARENT_SCOPE)
set(GLEW_BUILD_DIR ${GLEW}/src/${PROJECT_NAME} PARENT_SCOPE)
EXTERNALPROJECT_ADD(glew
    PREFIX ${GLEW_ROOT}

    URL https://sourceforge.net/projects/glew/files/glew/2.1.0/glew-2.1.0-win32.zip/download
    URL_MD5 32a72e6b43367db8dbea6010cd095355
	
	INSTALL_COMMAND ""    
	CONFIGURE_COMMAND ""
	PATCH_COMMAND ""
	BUILD_COMMAND ""
)
include_directories(${GLEW_ROOT}/include)
include_directories(${GLEW_BUILD_DIR} ${GLEW_INCLUDE_DIR} PARENT_SCOPE)

target_include_directories(Omen PRIVATE ${GLEW_INCLUDE_DIR})


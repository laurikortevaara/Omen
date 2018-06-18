project(boxer)

######################################################################
# BOXER
######################################################################
set(BOXER_ROOT ${CMAKE_BINARY_DIR}/thirdparty/${PROJECT_NAME})
set(BOXER_INCLUDE_DIR ${BOXER_ROOT}/src/Boxer/include)
set(BOXER_BUILD_DIR ${BOXER_ROOT}/src/${PROJECT_NAME} PARENT_SCOPE)
set(BOXER_LIBRARY ${BOXER_ROOT}/src/${PROJECT_NAME}/Debug/freetype.lib PARENT_SCOPE)
set(BOXER_LIBRARY_STATIC ${CMAKE_BINARY_DIR}/thirdparty/${PROJECT_NAME}/src/Boxer/Debug/Boxer.lib)
set(BOXER_LIBRARY_STATIC ${CMAKE_BINARY_DIR}/thirdparty/${PROJECT_NAME}/src/Boxer/Debug/Boxer.lib PARENT_SCOPE)
ExternalProject_Add(${PROJECT_NAME}
	PREFIX ${BOXER_ROOT}
	GIT_REPOSITORY "https://github.com/aaronmjacobs/Boxer.git"
	INSTALL_COMMAND ""
	PATCH_COMMAND ""	
	BUILD_IN_SOURCE ON
)
target_include_directories(Omen PRIVATE ${BOXER_INCLUDE_DIR})

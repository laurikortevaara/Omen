project(openal)
######################################################################
# OPENAL SDK
######################################################################
message("The project name: " ${PROJECT_NAME})
######################################################################
# OPENAL
######################################################################
set(OPENAL_ROOT ${CMAKE_BINARY_DIR}/thirdparty/${PROJECT_NAME})
set(OPENAL_INCLUDE_DIR ${OPENAL_ROOT}/src/${PROJECT_NAME} PARENT_SCOPE)
set(OPENAL_BUILD_DIR ${OPENAL_ROOT}/src/${PROJECT_NAME} PARENT_SCOPE)
ExternalProject_Add(${PROJECT_NAME}
	PREFIX ${OPENAL_ROOT}
	GIT_REPOSITORY "https://github.com/kcat/openal-soft.git"
	GIT_TAG	"openal-soft.1.15"
	INSTALL_COMMAND ""
	PATCH_COMMAND ""	
)
include_directories(${OPENAL_BUILD_DIR} ${OPENAL_INCLUDE_DIR} PARENT_SCOPE)



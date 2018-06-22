project(mono)
message("The project name: " ${PROJECT_NAME})
######################################################################
# MONO
######################################################################
set(MONO_ROOT ${CMAKE_BINARY_DIR}/thirdparty/${PROJECT_NAME})
set(MONO_INCLUDE_DIR ${MONO_ROOT}/src/${PROJECT_NAME} PARENT_SCOPE)
set(MONO_BUILD_DIR ${MONO_ROOT}/src/${PROJECT_NAME} PARENT_SCOPE)
ExternalProject_Add(${PROJECT_NAME}
	PREFIX ${MONO_ROOT}
	GIT_REPOSITORY "https://github.com/mono/mono.git"
	GIT_TAG "mono-5.12.0.260"
	PATCH_COMMAND ""
	CONFIGURE_COMMAND ""
	BUILD_COMMAND ""
	INSTALL_COMMAND ""
)
include_external_msproject( mono ${MONO_ROOT}/src/mono/msvc/mono.sln )
include_directories(${MONO_BUILD_DIR} ${MONO_INCLUDE_DIR} PARENT_SCOPE)


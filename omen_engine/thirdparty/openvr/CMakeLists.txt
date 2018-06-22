project(openvr)
######################################################################
# OpenVR SDK
######################################################################
message("The project name: " ${PROJECT_NAME})
######################################################################
# OPENVR
######################################################################
set(OPENVR_ROOT ${CMAKE_BINARY_DIR}/thirdparty/${PROJECT_NAME})
set(OPENVR_INCLUDE_DIR ${OPENVR_ROOT}/src/${PROJECT_NAME} PARENT_SCOPE)
set(OPENVR_BUILD_DIR ${OPENVR_ROOT}/src/${PROJECT_NAME} PARENT_SCOPE)
ExternalProject_Add(${PROJECT_NAME}
	PREFIX ${OPENVR_ROOT}
	GIT_REPOSITORY "https://github.com/ValveSoftware/openvr.git"
	GIT_TAG	"v1.0.15"
	INSTALL_COMMAND ""
	PATCH_COMMAND ""	
)
include_directories(${OPENVR_BUILD_DIR} ${OPENVR_INCLUDE_DIR} PARENT_SCOPE)



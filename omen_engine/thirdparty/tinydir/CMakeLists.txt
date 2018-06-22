project(tinydir)

######################################################################
# TINYDIR
######################################################################
set(TINYDIR_ROOT ${CMAKE_BINARY_DIR}/thirdparty/${PROJECT_NAME})
set(TINYDIR_INCLUDE_DIR ${TINYDIR_ROOT}/src/${PROJECT_NAME} PARENT_SCOPE)
set(TINYDIR_BUILD_DIR ${TINYDIR_ROOT}/src/${PROJECT_NAME} PARENT_SCOPE)
ExternalProject_Add(${PROJECT_NAME}
	PREFIX ${TINYDIR_ROOT}
	GIT_REPOSITORY "https://github.com/cxong/tinydir.git"
	GIT_TAG "1.2.3"
	PATCH_COMMAND ""
	CONFIGURE_COMMAND ""
	BUILD_COMMAND ""
	INSTALL_COMMAND ""	
)
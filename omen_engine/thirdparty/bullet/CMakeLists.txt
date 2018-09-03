project(bullet)

######################################################################
# BULLET_PHYSICS
######################################################################
set(BULLET_PHYSICS_ROOT ${CMAKE_BINARY_DIR}/thirdparty/${PROJECT_NAME})
set(BULLET_PHYSICS_INCLUDE_DIR ${BULLET_PHYSICS_ROOT}/src/${PROJECT_NAME} PARENT_SCOPE)
set(BULLET_PHYSICS_BUILD_DIR ${BULLET_PHYSICS_ROOT}/src/${PROJECT_NAME} PARENT_SCOPE)
ExternalProject_Add(${PROJECT_NAME}
	PREFIX ${BULLET_PHYSICS_ROOT}
	GIT_REPOSITORY "https://github.com/bulletphysics/bullet3.git"
	GIT_TAG	"2.87"
	INSTALL_COMMAND ""
	PATCH_COMMAND ""	
)
include_directories(${BULLET_PHYSICS_BUILD_DIR} ${BULLET_PHYSICS_INCLUDE_DIR} PARENT_SCOPE)



project(glm)

######################################################################
# GLM
######################################################################
set(GLM_ROOT ${CMAKE_BINARY_DIR}/thirdparty/${PROJECT_NAME})
set(GLM_BUILD_DIR ${GLM_ROOT}/src/${PROJECT_NAME} PARENT_SCOPE)
set(GLM_INCLUDE_DIR ${GLM_ROOT}/src/${PROJECT_NAME} PARENT_SCOPE)
set(GLM_BUILD_DIR ${GLM_ROOT}/src/${PROJECT_NAME})
set(GLM_INCLUDE_DIR ${GLM_ROOT}/src/${PROJECT_NAME})

ExternalProject_Add(${PROJECT_NAME}
	PREFIX ${GLM_ROOT}
	GIT_REPOSITORY "https://github.com/g-truc/glm.git"
	PATCH_COMMAND ""
	CONFIGURE_COMMAND ""
	BUILD_COMMAND ""
	INSTALL_COMMAND ""	
)

target_include_directories(Omen PRIVATE ${GLM_INCLUDE_DIR})


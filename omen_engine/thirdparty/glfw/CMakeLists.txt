project(glfw)

######################################################################
# GLFW
######################################################################
set(GLFW_ROOT ${CMAKE_BINARY_DIR}/thirdparty/${PROJECT_NAME})
set(GLFW_INCLUDE_DIR ${GLFW_ROOT}/src/glfw/include)
set(GLFW_INCLUDE_DIR ${GLFW_ROOT}/src/glfw/include PARENT_SCOPE)
set(GLFW_LIBRARY_STATIC ${CMAKE_BINARY_DIR}/thirdparty/glfw/src/glfw/src/Debug/glfw3.lib PARENT_SCOPE)
set(GLFW_BUILD_DIR ${GLFW_ROOT}/src/${PROJECT_NAME} PARENT_SCOPE)
ExternalProject_Add(${PROJECT_NAME}
	PREFIX ${GLFW_ROOT}
	GIT_REPOSITORY "https://github.com/glfw/glfw.git"
	GIT_TAG "3.2.1"
	INSTALL_COMMAND ""
	PATCH_COMMAND ""	
	BUILD_IN_SOURCE 1
)


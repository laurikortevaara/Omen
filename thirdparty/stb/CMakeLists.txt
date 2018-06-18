project(stb)

######################################################################
# STB
######################################################################
set(STB_ROOT ${CMAKE_BINARY_DIR}/thirdparty/${PROJECT_NAME})
set(STB_INCLUDE_DIR ${STB_ROOT}/src/STB)
set(STB_BUILD_DIR ${STB_ROOT}/src/${PROJECT_NAME} PARENT_SCOPE)
ExternalProject_Add(${PROJECT_NAME}
	PREFIX ${STB_ROOT}
	GIT_REPOSITORY "https://github.com/nothings/stb.git"
	PATCH_COMMAND ""
	CONFIGURE_COMMAND ""
	BUILD_COMMAND ""
	INSTALL_COMMAND ""	
)
include_directories(${STB_BUILD_DIR} ${STB_INCLUDE_DIR} PARENT_SCOPE)
target_include_directories(Omen PRIVATE ${STB_INCLUDE_DIR})

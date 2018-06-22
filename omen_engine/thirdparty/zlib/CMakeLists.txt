project(zlib)

######################################################################
# ZLIB
######################################################################
set(ZLIB_ROOT ${CMAKE_BINARY_DIR}/thirdparty/${PROJECT_NAME})
set(ZLIB_INCLUDE_DIR ${ZLIB_ROOT}/src/${PROJECT_NAME})
set(ZLIB_BUILD_DIR ${ZLIB_ROOT}/src/${PROJECT_NAME} PARENT_SCOPE)
set(ZLIB_LIBRARY ${ZLIB_ROOT}/src/${PROJECT_NAME}/Debug/zlibstaticd.lib PARENT_SCOPE)
set(ZLIB_LIBRARY_STATIC ${CMAKE_BINARY_DIR}/thirdparty/${PROJECT_NAME}/src/zlib/Debug/zlibstaticd.lib)
set(ZLIB_LIBRARY_STATIC ${CMAKE_BINARY_DIR}/thirdparty/${PROJECT_NAME}/src/zlib/Debug/zlibstaticd.lib PARENT_SCOPE)
ExternalProject_Add(${PROJECT_NAME}
	PREFIX ${ZLIB_ROOT}
	GIT_REPOSITORY "https://github.com/madler/zlib"
	BUILD_IN_SOURCE ON
	GIT_TAG "v1.2.10"
	CMAKE_ARGS += -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} -DBUILD_TESTING=OFF
)
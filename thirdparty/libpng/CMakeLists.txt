project (libpng)

######################################################################
# LIBPNG
######################################################################
set(LIBPNG_ROOT ${CMAKE_BINARY_DIR}/thirdparty/${PROJECT_NAME})
set(LIBPNG_ROOT ${CMAKE_BINARY_DIR}/thirdparty/${PROJECT_NAME} PARENT_SCOPE)
set(LIBPNG_INCLUDE_DIR ${LIBPNG_ROOT}/src/libpng)
set(LIBPNG_INCLUDE_DIR ${LIBPNG_ROOT}/src/libpng PARENT_SCOPE)
set(LIBPNG_LIBRARY_STATIC ${CMAKE_BINARY_DIR}/thirdparty/${PROJECT_NAME}/src/libpng/Debug/libpng17_staticd.lib)
set(LIBPNG_LIBRARY_STATIC ${CMAKE_BINARY_DIR}/thirdparty/${PROJECT_NAME}/src/libpng/Debug/libpng17_staticd.lib PARENT_SCOPE)
set(LIBPNG_BUILD_DIR ${LIBPNG}/src/${PROJECT_NAME} PARENT_SCOPE)


ExternalProject_Add(libpng
	DEPENDS zlib
	PREFIX ${LIBPNG_ROOT}
	GIT_REPOSITORY "git://git.code.sf.net/p/libpng/code"
	GIT_TAG "v1.7.0beta89"
	BUILD_IN_SOURCE ON
	CMAKE_ARGS += -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} 
	CMAKE_ARGS += -DBUILD_TESTING=OFF 
	CMAKE_ARGS += -DZLIB_INCLUDE_DIR=${ZLIB_BUILD_DIR}
	CMAKE_ARGS += -DZLIB_LIBRARY=${ZLIB_LIBRARY}
	CMAKE_ARGS += -I${ZLIB_BUILD_DIR}
)
include_directories(${LIBPNG_ROOT})
include(fix_libpng.cmake)

target_include_directories(Omen PRIVATE ${LIBPNG_INCLUDE_DIR})
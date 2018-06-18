#add zlib build directory to include dirs to allow inclusion of zconf.h generated during build
include_directories(${ZLIB_BUILD_DIR} ${ZLIB_INCLUDE_DIR})
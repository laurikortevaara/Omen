project(freetype)

######################################################################
# freetype
######################################################################
set(FREETYPE_ROOT ${CMAKE_BINARY_DIR}/thirdparty/${PROJECT_NAME})
set(FREETYPE_INCLUDE_DIR ${FREETYPE_ROOT}/src/freetype/include)
set(FREETYPE_INCLUDE_DIR ${FREETYPE_ROOT}/src/freetype/include PARENT_SCOPE)
set(FREETYPE_BUILD_DIR ${FREETYPE}/src/${PROJECT_NAME} PARENT_SCOPE)
set(FREETYPE_LIBRARY ${FREETYPE_ROOT}/src/${PROJECT_NAME}/Debug/freetype.lib PARENT_SCOPE)
set(FREETYPE_LIBRARY_STATIC ${CMAKE_BINARY_DIR}/thirdparty/${PROJECT_NAME}/src/freetype/Debug/freetype.lib)
set(FREETYPE_LIBRARY_STATIC ${CMAKE_BINARY_DIR}/thirdparty/${PROJECT_NAME}/src/freetype/Debug/freetype.lib PARENT_SCOPE)
EXTERNALPROJECT_ADD(freetype
    PREFIX ${FREETYPE_ROOT}

    URL http://download.savannah.gnu.org/releases/freetype/freetype-2.6.tar.gz
    URL_MD5 1d733ea6c1b7b3df38169fbdbec47d2b
   
	INSTALL_COMMAND ""    
	BUILD_IN_SOURCE ON
)

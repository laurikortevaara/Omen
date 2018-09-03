project(fbxsdk)

######################################################################
# FBXSDK
######################################################################
find_package(FbxSdk REQUIRED)
if(NOT FBX_INCLUDE_DIR)
    message(FATAL_ERROR "FBX SDK not found: " ${FBX_INCLUDE_DIR})
endif (NOT FBX_INCLUDE_DIR)

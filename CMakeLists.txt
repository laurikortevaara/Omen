cmake_minimum_required(VERSION 3.3)
project(Omen)

set(CMAKE_C_COMPILER "/usr/bin/clang")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O2 -Wall -pedantic -stdlib=libc++")

# FORCE OS X 10.8 DEPLOYMENT TARGET ON OS X TO SUPPORT GLUT
if(APPLE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.8")
endif(APPLE)

#########################################################
# FIND GLUT
#########################################################
find_package(GLUT REQUIRED)
include_directories(${GLUT_INCLUDE_DIRS})
link_directories(${GLUT_LIBRARY_DIRS})
add_definitions(${GLUT_DEFINITIONS})
if(NOT GLUT_FOUND)
    message(ERROR " GLUT not found!")
endif(NOT GLUT_FOUND)


#########################################################
# FIND OPENGL
#########################################################
find_package(OpenGL REQUIRED)
include_directories(${OpenGL_INCLUDE_DIRS})
link_directories(${OpenGL_LIBRARY_DIRS})
add_definitions(${OpenGL_DEFINITIONS})
if(NOT OPENGL_FOUND)
    message(ERROR " OPENGL not found!")
endif(NOT OPENGL_FOUND)


#########################################################
# GLFW
#########################################################
add_subdirectory(thirdparty/glfw)
include_directories(thirdparty/glfw/include)

#########################################################
# GLM
#########################################################
add_subdirectory(thirdparty/glm)
include_directories(thirdparty/glm)

#########################################################
# STB
#########################################################
#add_subdirectory(thirdparty/stb)
include_directories(thirdparty)

set(SOURCE_FILES main.cpp Model.cpp Model.h Mesh.cpp Mesh.h Triangle.cpp Triangle.h Vertex.cpp Vertex.h VertexArrayObject.cpp VertexArrayObject.h VertexBufferObject.cpp VertexBufferObject.h Scene.cpp Scene.h GL_error.h.h Camera.cpp Camera.h Signal.h)
add_executable(Omen ${SOURCE_FILES})

TARGET_LINK_LIBRARIES(Omen ${OPENGL_LIBRARIES})
TARGET_LINK_LIBRARIES(Omen ${GLUT_LIBRARIES})
TARGET_LINK_LIBRARIES(Omen glfw ${GLFW_LIBRARIES})

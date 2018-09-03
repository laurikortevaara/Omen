######################################################################
# omen Game engine project
#
# Lauri Kortevaara (2015-2018)
# lauri.kortevaara@gmail.com
#
# https://github.com/laurikortevaara/omen
#
######################################################################
cmake_minimum_required(VERSION 3.9.6)
set(MSVC_INCREMENTAL_DEFAULT ON)
set(CMAKE_CONFIGURATION_TYPES "Debug;Release;RelWithDebInfo" CACHE PATH "Configuration types")

project(Omen)

######################################################################
# The Omen engine
######################################################################
add_subdirectory(omen_engine)
message("Omen Include dirs in parent cmake: " ${OMEN_INCLUDE_DIRS})
include_directories(${OMEN_INCLUDE_DIRS})

######################################################################
# Sample omen engine projects
######################################################################
add_subdirectory(samples/01_hello_omen_engine)
add_dependencies(01_hello_omen_engine OmenEngineShared)

add_subdirectory(samples/02_omen_engine_main_loop)
add_dependencies(02_omen_engine_main_loop OmenEngineShared)
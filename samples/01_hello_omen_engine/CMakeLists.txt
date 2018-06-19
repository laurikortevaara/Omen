project(01_hello_omen_engine)
set(TARGET_NAME 01_hello_omen_engine)

if(APPLE)
  if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set_source_files_properties(hello_omen_engine_main.cpp PROPERTIES COMPILE_FLAGS "-x objective-c++")
    find_library(FOUNDATION_FRAMEWORK Foundation)
    mark_as_advanced(FOUNDATION_FRAMEWORK)
    set(EXTRA_LIBS ${EXTRA_LIBS} ${FOUNDATION_FRAMEWORK})
  endif()
endif()

add_executable(${TARGET_NAME}
  ${SHARED_SRC_FILES}
  hello_omen_engine_main.cpp
)

target_link_libraries(${TARGET_NAME}
  ${OMEN_LIBRARY_NAME}
)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

set_target_properties(${TARGET_NAME} PROPERTIES
                      VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
                      CXX_STANDARD 17
                      )

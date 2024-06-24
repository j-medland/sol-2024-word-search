cmake_minimum_required(VERSION 3.5)

project(wss VERSION 1.0)

SET(INSTALL_DIR ${CMAKE_CURRENT_LIST_DIR})

# enforce MSVC only on Windows
if(WIN32 AND !MSVC)
  error("Only MSVC compiler support is currently provided by this CMake tooling.")
endif()

# find labview library and cintools
include(${CMAKE_CURRENT_SOURCE_DIR}/find_labview_library.cmake)

# Specify the library
add_library(${PROJECT_NAME} SHARED "word-search-solver.cpp")

set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 20)

# turn off safe-exception-handling on windows as it is not supported by labview(v).lib
if(WIN32)
  SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /SAFESEH:NO")
endif(WIN32)

if(UNIX)
  # don't use "lib"-prefix on linux
  set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")
endif()

# create the cross-platform export header definition
include(GenerateExportHeader)
generate_export_header(${PROJECT_NAME})

# put bitness into output name
math(EXPR BITNESS "8*${CMAKE_SIZEOF_VOID_P}")
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "${PROJECT_NAME}.${BITNESS}")

# set the libraries to link against
target_link_libraries(${PROJECT_NAME} PRIVATE labview)

# add the include dirs
target_include_directories(${PROJECT_NAME} PRIVATE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}> # add the build directory to include the auto-generated export header
)

# define the install targets for the extension library
install(TARGETS ${PROJECT_NAME}
  RUNTIME DESTINATION ${INSTALL_DIR}
  LIBRARY DESTINATION ${INSTALL_DIR}
)
cmake_minimum_required(VERSION 3.5)

project(wss VERSION 1.0)

SET(INSTALL_DIR ${CMAKE_CURRENT_LIST_DIR})

# Specify the library
add_library(${PROJECT_NAME} SHARED "word-search-solver.cpp")

# C++ 20 to get std::span
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 20)

if(WIN32)
  # set the alignment of bytes for types on Windows 32-bit
  if(CMAKE_SIZEOF_VOID_P EQUAL 4)
    target_compile_definitions(${PROJECT_NAME} PRIVATE ALIGN_BYTES_1)
  endif()
endif(WIN32)
if(UNIX)
  # don't use "lib"-prefix on linux
  set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")
endif()

# create the cross-platform export header definition
include(GenerateExportHeader)
generate_export_header(${PROJECT_NAME})

# put bitness into output name so labVIEW can automatically select the correct .dll/.so for the platform
math(EXPR BITNESS "8*${CMAKE_SIZEOF_VOID_P}")
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "${PROJECT_NAME}.${BITNESS}")


# add the include dirs
target_include_directories(${PROJECT_NAME} PRIVATE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}> # add the build directory to include the auto-generated export header
)

# define the install targets for the extension library
install(TARGETS ${PROJECT_NAME}
  RUNTIME DESTINATION ${INSTALL_DIR}
  LIBRARY DESTINATION ${INSTALL_DIR}
)
cmake_minimum_required(VERSION 3.0.2)

include(ExternalProject)

set(CMAKE_CXX_STANDARD 17)

project(chibi_bindings)
add_library(chibi_bindings
  include/sexp.hpp
  include/Chibi.hpp

  src/SExp.cpp
  src/Chibi.cpp
  src/RTD.cpp
)
target_include_directories(chibi_bindings
  PUBLIC include
  PUBLIC src
)

set(CHIBI_PATH ${CMAKE_CURRENT_SOURCE_DIR}/deps/chibi-scheme)

# Build chibi, all libraries and an image
add_custom_target(chibi-scheme
  COMMAND make libchibi-scheme${CMAKE_SHARED_LIBRARY_SUFFIX} all-libs CC=${CMAKE_C_COMPILER} CHIBI_MODULE_PATH=${CHIBI_PATH}/lib
  WORKING_DIRECTORY ${CHIBI_PATH}
)
# Make the main library depend on the custom command
add_dependencies(chibi_bindings chibi-scheme)
if (UNIX)
  target_link_libraries(chibi_bindings dl) # Only need to link in dl on unix
endif()

# Copy the shared library to the final output directory
add_custom_command(
  COMMENT "Copying libchibi-scheme to the build output directory"
  TARGET chibi-scheme POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
  ${CHIBI_PATH}/libchibi-scheme${CMAKE_SHARED_LIBRARY_SUFFIX}.0
  ${CMAKE_BINARY_DIR}/libchibi-scheme${CMAKE_SHARED_LIBRARY_SUFFIX}.0
)

# Copy all the runtime things to the final output dir
add_custom_command(
  COMMENT "Copying runtime libraries to the build output directory"
  TARGET chibi-scheme POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
  ${CHIBI_PATH}/lib
  ${CMAKE_BINARY_DIR}/chibi_lib
)

# Import the shared library into CMake
add_library(libchibi-scheme SHARED IMPORTED)
set_target_properties(libchibi-scheme PROPERTIES
  IMPORTED_LOCATION ${CMAKE_BINARY_DIR}/libchibi-scheme${CMAKE_SHARED_LIBRARY_SUFFIX}.0
)

# Link to the imported library
target_link_libraries(chibi_bindings libchibi-scheme)
target_include_directories(chibi_bindings
  PUBLIC deps/chibi-scheme/include
)

# Testing

include(CTest)
include(GoogleTest)
find_package(GTest)
add_executable(chibi_bindings_tests
  test/ChibiTests.cpp
  test/SExpTests.cpp
  test/RTDTests.cpp
)
target_link_libraries(chibi_bindings_tests ${GTEST_BOTH_LIBRARIES} chibi_bindings)
gtest_discover_tests(chibi_bindings_tests)

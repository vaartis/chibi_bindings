cmake_minimum_required(VERSION 3.0.2)

include(ExternalProject)

set(CMAKE_CXX_STANDARD 17)

option(CHIBI_BINDINGS_BUILD_TESTS "Build tests" NO)

project(chibi_bindings)
add_library(chibi_bindings
  src/SExp.cpp
  src/Chibi.cpp
  src/RTD.cpp
)
target_include_directories(chibi_bindings
  PUBLIC include
  PUBLIC src
  )
target_compile_options(chibi_bindings PUBLIC -Wno-array-bounds) # Works incorrectly with chibi

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

if (CHIBI_BINDINGS_BUILD_TESTS)
  include(CTest)
  include(GoogleTest)

  set(BUILD_GMOCK NO CACHE BOOL "Don't build GMock" FORCE)
  set(INSTALL_GTEST NO CACHE BOOL "Don't install GTest" FORCE)
  add_subdirectory(deps/gtest)
  set_target_properties(gtest_main PROPERTIES # Make gtest NOT create lib/bin directories
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
    PDB_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
    )
  set_target_properties(gtest PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
    PDB_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
    )
  add_executable(chibi_bindings_tests
    test/ChibiTests.cpp
    test/SExpTests.cpp
    test/RTDTests.cpp
    test/ClassRegistratorTests.cpp
    test/SExpConvertersTests.cpp
    )
  target_link_libraries(chibi_bindings_tests gtest_main chibi_bindings)
  gtest_discover_tests(chibi_bindings_tests)
endif()

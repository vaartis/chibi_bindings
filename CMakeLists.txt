cmake_minimum_required(VERSION 3.0.2)

include(ExternalProject)

set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS YES)

project(chibi_bindings)
add_library(chibi_bindings src/Chibi.cpp)
include_directories(include)

ExternalProject_Add(chibi_scheme
  GIT_REPOSITORY "https://github.com/ashinn/chibi-scheme"
  GIT_TAG "0.8"

  SOURCE_DIR "${CMAKE_SOURCE_DIR}/deps/download/chibi"

  CONFIGURE_COMMAND ""
  BUILD_COMMAND "make"
  INSTALL_COMMAND make install DESTDIR="${CMAKE_SOURCE_DIR}/deps/chibi" PREFIX=""
  BUILD_IN_SOURCE 1

  LOG_BUILD 1
  )
link_directories("${CMAKE_SOURCE_DIR}/deps/chibi/lib")
target_link_libraries(chibi_bindings "chibi-scheme")
include_directories("${CMAKE_SOURCE_DIR}/deps/chibi/include")
add_dependencies(chibi_bindings chibi_scheme)

include(GoogleTest)
find_package(GTest)
add_executable(chibi_bindings_tests test/ChibiTests.cpp)
target_link_libraries(chibi_bindings_tests ${GTEST_BOTH_LIBRARIES} chibi_bindings)
gtest_discover_tests(chibi_bindings_tests)

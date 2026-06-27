cmake_minimum_required(VERSION 3.15...3.27)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(pwcoco VERSION 1.2.0 LANGUAGES CXX)

include(CTest)

if(NOT SKBUILD)
  message(WARNING "Standalone CMake build. For Python packaging use: pip install -ve .")
endif()

if(DEFINED "$ENV{EIGEN3_INCLUDE_DIR}")
  set(EIGEN3_INCLUDE_DIR "$ENV{EIGEN3_INCLUDE_DIR}")
  if(NOT EXISTS EIGEN3_INCLUDE_DIR)
    message(FATAL_ERROR "Incorrect environment variable EIGEN3_INCLUDE_DIR for the location of Eigen3 package")
  endif()
endif()

if(DEFINED "$ENV{SPDLOG_INCLUDE_DIR}")
  set(SPDLOG_INCLUDE_DIR "$ENV{SPDLOG_INCLUDE_DIR}")
  if(NOT EXISTS SPDLOG_INCLUDE_DIR)
    message(FATAL_ERROR "Incorrect environment variable SPDLOG_INCLUDE_DIR for the location of spdlog package")
  endif()
endif()

if(DEFINED "$ENV{EIGEN3_INCLUDE_DIR}" AND DEFINED "$ENV{SPDLOG_INCLUDE_DIR}")
  include_directories("${EIGEN3_INCLUDE_DIR}")
  include_directories("${SPDLOG_INCLUDE_DIR}")
else()
  include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include/")
endif()

set(PWCOCO_CORE_SOURCES
  src/pwcoco_run.cpp
  src/coloc.cpp
  src/conditional.cpp
  src/data.cpp
  src/dcdflib.cpp
  src/helper_funcs.cpp
)

add_library(pwcoco_core STATIC ${PWCOCO_CORE_SOURCES})
target_compile_features(pwcoco_core PUBLIC cxx_std_17)
target_include_directories(pwcoco_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
set_target_properties(pwcoco_core PROPERTIES POSITION_INDEPENDENT_CODE ON)
if(UNIX AND NOT APPLE AND NOT WIN32)
  target_link_libraries(pwcoco_core PRIVATE stdc++fs)
endif()

find_package(OpenMP)
if(OpenMP_CXX_FOUND)
  message(STATUS "Linking OpenMP")
  target_link_libraries(pwcoco_core PUBLIC OpenMP::OpenMP_CXX)
endif()

if(APPLE AND OpenMP_CXX_FOUND)
  target_link_options(pwcoco_core PUBLIC "-Wl,-rpath,/opt/homebrew/opt/libomp/lib")
endif()

add_library(pwcoco_cli STATIC src/cli.cpp)
target_include_directories(pwcoco_cli PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_executable(pwcoco src/options.cpp)
target_link_libraries(pwcoco PRIVATE pwcoco_cli pwcoco_core)

if(SKBUILD)
  find_package(Python COMPONENTS Interpreter Development.Module REQUIRED)
  find_package(nanobind CONFIG REQUIRED)

  nanobind_add_module(_pwcoco python/bindings.cpp)
  target_link_libraries(_pwcoco PRIVATE pwcoco_core)
  target_include_directories(_pwcoco PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

  install(TARGETS _pwcoco LIBRARY DESTINATION pwcoco)
  install(TARGETS pwcoco RUNTIME DESTINATION pwcoco/bin)
endif()

if(BUILD_TESTING AND NOT SKBUILD)
  include(FetchContent)
  FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
  )
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
  FetchContent_MakeAvailable(googletest)

  add_executable(
    pwcoco_tests
    tests/cpp/test_options.cpp
    tests/cpp/test_helper_funcs.cpp
  )
  target_link_libraries(pwcoco_tests PRIVATE pwcoco_cli pwcoco_core GTest::gtest_main)
  include(GoogleTest)
  gtest_discover_tests(pwcoco_tests)
endif()

cmake_minimum_required(VERSION 4.2) 

if(CMAKE_VERSION VERSION_GREATER_EQUAL 4.3)
  set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "451f2fe2-a8a2-47c3-bc32-94786d8fc91b")
elseif(CMAKE_VERSION VERSION_GREATER_EQUAL 4.2)
  set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "d0edc3af-4c50-42ea-a356-e2862fe7a444")
endif()

cmake_language(GET_EXPERIMENTAL_FEATURE_ENABLED "CxxImportStd" feature_present)

if(NOT feature_present STREQUAL "TRUE")
  message(FATAL_ERROR "CMake import std support not enabled!?")
endif()

include(CMakeOptions.txt)

if(NOT CMAKE_BUILD_TYPE)
  message(WARNING "No build type specified, assuming 'Debug' build")
  set(CACHE{CMAKE_BUILD_TYPE} VALUE "Debug")
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
     "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()


set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS FALSE)
set(CMAKE_CXX_MODULE_STD ON)

project(lmtd LANGUAGES CXX)

add_subdirectory(inate)

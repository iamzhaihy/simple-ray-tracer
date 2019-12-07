cmake_minimum_required(VERSION 3.0.0)
project(simple-ray-tracer CXX)

# output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

# compilation flags: adapt to your needs
if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP /bigobj")
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR})
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR})
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

# add src to the include directories
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src")

# include GLM for linear algebra
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/libs/glm")

# include stb for writing images
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/libs/stb")

# include RapisJson for parsing JSON files
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/libs/rapidjson/include")

# compile all the cpp files in src
file(GLOB SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_executable(${PROJECT_NAME}_bin ${SOURCES})

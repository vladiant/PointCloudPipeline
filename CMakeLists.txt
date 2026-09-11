cmake_minimum_required(VERSION 3.20)
project(PointCloudPipeline VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# --- Options (design section 4.1) --------------------------------------------
# PCP_ENABLE_VIZ gates only the optional Open3D interactive path. The default
# custom PPM offscreen renderer (pcp_viz) is always built and needs no deps.
option(PCP_ENABLE_VIZ    "Enable the optional Open3D interactive viz path" OFF)
option(PCP_ENABLE_SPDLOG "Use spdlog for logging"                          ON)
option(PCP_BUILD_TESTS   "Build unit tests"                                ON)

if(PCP_ENABLE_SPDLOG)
  find_package(spdlog REQUIRED)
endif()

if(PCP_ENABLE_VIZ)
  find_package(Open3D REQUIRED)
endif()

include(GNUInstallDirs)
enable_testing()

add_subdirectory(src/core)
add_subdirectory(src/io)
add_subdirectory(src/index)
add_subdirectory(src/query)
add_subdirectory(src/bench)
add_subdirectory(src/viz)
add_subdirectory(src/cli)

if(PCP_BUILD_TESTS)
  find_package(GTest REQUIRED)
  add_subdirectory(tests)
endif()

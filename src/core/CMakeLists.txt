add_library(pcp_core STATIC
    types.cpp
    hardware_info.cpp
    synthetic.cpp
)

target_include_directories(pcp_core PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_compile_features(pcp_core PUBLIC cxx_std_17)
target_compile_definitions(pcp_core PRIVATE PCP_BUILD_TYPE="$<CONFIG>")

if(PCP_ENABLE_SPDLOG)
  # PRIVATE: logging is an implementation detail and must not leak to consumers.
  target_link_libraries(pcp_core PRIVATE spdlog::spdlog)
  target_compile_definitions(pcp_core PRIVATE PCP_HAS_SPDLOG)
endif()

add_library(pcp_bench STATIC harness.cpp)
target_link_libraries(pcp_bench PUBLIC pcp_core pcp_query)

if(PCP_ENABLE_SPDLOG)
  target_link_libraries(pcp_bench PRIVATE spdlog::spdlog)
endif()

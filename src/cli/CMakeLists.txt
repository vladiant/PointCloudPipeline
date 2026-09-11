add_executable(pcp_cli main.cpp args.cpp)
target_link_libraries(pcp_cli PRIVATE
    pcp_io pcp_index pcp_query pcp_bench pcp_viz)

if(PCP_ENABLE_VIZ)
  target_compile_definitions(pcp_cli PRIVATE PCP_ENABLE_VIZ)
endif()

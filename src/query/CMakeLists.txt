add_library(pcp_query STATIC queries.cpp)
target_link_libraries(pcp_query PUBLIC pcp_core pcp_index)

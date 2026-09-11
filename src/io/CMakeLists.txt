add_library(pcp_io STATIC loader.cpp)
target_link_libraries(pcp_io PUBLIC pcp_core)

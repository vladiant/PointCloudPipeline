add_library(pcp_index STATIC
    kd_tree_3d.cpp
    brute_force_index.cpp
)
target_link_libraries(pcp_index PUBLIC pcp_core)

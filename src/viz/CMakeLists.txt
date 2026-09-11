# pcp_viz is ALWAYS built: the default custom PPM renderer needs no third-party
# dependency, satisfying the PM decision that an offscreen image is the default
# output. PCP_ENABLE_VIZ additionally compiles the optional Open3D interactive
# path inside the same target (design OQ-4). This is a deliberate, documented
# deviation from the literal section 4.2 gating (which would omit pcp_viz
# entirely when OFF); see NOTICES.md / README deviations.
add_library(pcp_viz STATIC renderer.cpp)
target_link_libraries(pcp_viz PUBLIC pcp_core pcp_index)

if(PCP_ENABLE_VIZ)
  target_link_libraries(pcp_viz PRIVATE Open3D::Open3D)
  target_compile_definitions(pcp_viz PRIVATE PCP_ENABLE_VIZ)
endif()

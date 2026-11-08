# Portable 3D asset code (no GPU dependency): meshes, skeletons, animation, DDS.
add_library(joan_en3d STATIC
    model.cpp
    animation.cpp
    dds.cpp
    asset_paths.cpp
)
target_link_libraries(joan_en3d PUBLIC joan_core PRIVATE joan_warnings)

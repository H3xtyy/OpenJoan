add_library(joan_render STATIC gl.cpp model_renderer.cpp)
target_link_libraries(joan_render PUBLIC joan_core joan_en3d PRIVATE joan_warnings)

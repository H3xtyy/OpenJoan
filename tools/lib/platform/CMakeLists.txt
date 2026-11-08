add_library(joan_platform STATIC window.cpp)
target_link_libraries(joan_platform PUBLIC joan_core SDL3::SDL3 PRIVATE joan_warnings)

add_library(joan_core STATIC
    log.cpp
    vfs.cpp
    file_util.cpp
)
target_include_directories(joan_core PUBLIC ${PROJECT_SOURCE_DIR}/tools/lib)
target_link_libraries(joan_core PRIVATE joan_warnings)

add_library(joan_ilib STATIC
    res_archive.cpp
    database.cpp
    game_set.cpp
    kv_text.cpp
)
target_link_libraries(joan_ilib PUBLIC joan_core PRIVATE joan_warnings)

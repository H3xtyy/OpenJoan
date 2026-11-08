add_executable(joan main.cpp)
target_link_libraries(joan PRIVATE joan_ilib joan_platform joan_render joan_warnings)
if(WIN32)
    set_target_properties(joan PROPERTIES WIN32_EXECUTABLE $<CONFIG:Release>)
    # Copy SDL3.dll (and any other runtime DLLs) next to joan.exe so it starts from the build folder.
    add_custom_command(TARGET joan POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_RUNTIME_DLLS:joan> $<TARGET_FILE_DIR:joan>
        COMMAND_EXPAND_LISTS)
endif()

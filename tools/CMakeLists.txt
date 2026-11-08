foreach(tool jres jdbf)
    add_executable(${tool} ${tool}.cpp)
    target_link_libraries(${tool} PRIVATE joan_ilib joan_warnings)
endforeach()

add_executable(jmodel jmodel.cpp)
target_link_libraries(jmodel PRIVATE joan_en3d joan_warnings)

# Model / animation viewer: needs SDL3 + OpenGL (built with the engine).
if(TARGET joan_platform AND TARGET joan_render)
    add_executable(jview jview.cpp)
    target_link_libraries(jview PRIVATE joan_en3d joan_platform joan_render joan_warnings)
    if(WIN32)
        add_custom_command(TARGET jview POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_RUNTIME_DLLS:jview> $<TARGET_FILE_DIR:jview>
            COMMAND_EXPAND_LISTS)
    endif()
endif()

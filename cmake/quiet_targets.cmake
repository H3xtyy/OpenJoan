# joan_quiet_targets(<source dir>): third-party code built with the project (OpenAL Soft, SDL3) compiles with warnings
# off - its warnings are not ours to fix and would break the zero-warning build log. With MSVC its own warning options
# (also PUBLIC / INTERFACE ones, which otherwise land after ours) are removed and the level is /W0, not /w: the Visual
# Studio generator turns /W0 into the project's warning level (TurnOffAllWarnings) but passes /w on as an extra option
# next to the default level (cl: "D9025 overriding '/W1' with '/w'").
include_guard(GLOBAL)
function(joan_quiet_targets dir)
    get_property(_targets DIRECTORY "${dir}" PROPERTY BUILDSYSTEM_TARGETS)
    foreach(_t IN LISTS _targets)
        get_target_property(_type ${_t} TYPE)
        if(MSVC)
            # Its own /W4 /wd... (also PUBLIC ones: alsoft.common passes them to every target linking it, after
            # ours) go, and the level is /W0, not /w: the Visual Studio generator turns /W0 into the project's
            # warning level (TurnOffAllWarnings), but passes /w on as an extra option next to the default level
            # (cl: "D9025 overriding '/W1' with '/w'").
            foreach(_prop COMPILE_OPTIONS INTERFACE_COMPILE_OPTIONS)
                get_target_property(_opts ${_t} ${_prop})
                if(_opts)
                    # also inside generator expressions ($<$<COMPILE_LANGUAGE:CXX>:/W4> ...)
                    list(FILTER _opts EXCLUDE REGEX "(^|[:;])/[Ww][0-9]")
                    list(FILTER _opts EXCLUDE REGEX "(^|[:;])/w[de1-4][0-9]+")
                    set_target_properties(${_t} PROPERTIES ${_prop} "${_opts}")
                endif()
            endforeach()
        endif()
        if(_type MATCHES "^(STATIC_LIBRARY|SHARED_LIBRARY|MODULE_LIBRARY|OBJECT_LIBRARY|EXECUTABLE)$")
            if(MSVC)
                # /wd4267: OpenAL Soft's x64 build still reported C4267 (size_t -> unsigned in <utility>, an
                # instantiation from its sources) under /W0 - disabled by number as well
                target_compile_options(${_t} PRIVATE /W0 /wd4267)
            else()
                target_compile_options(${_t} PRIVATE -w)
            endif()
        endif()
    endforeach()
    get_property(_subdirs DIRECTORY "${dir}" PROPERTY SUBDIRECTORIES)
    foreach(_d IN LISTS _subdirs)
        joan_quiet_targets("${_d}")
    endforeach()
endfunction()

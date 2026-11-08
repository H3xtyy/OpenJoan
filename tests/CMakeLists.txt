add_executable(joan_tests test_ilib.cpp test_en3d.cpp)
target_link_libraries(joan_tests PRIVATE joan_ilib joan_en3d joan_warnings)
add_test(NAME joan_tests COMMAND joan_tests)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_DEBUG} -w -O0 -g3 -no-pie")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS_DEBUG} -w -O0 -g3 -no-pie")

if (CMAKE_COMPILER_IS_GNUCXX)
    setup_code_coverage()
    append_coverage_compiler_flags()
endif ()

add_subdirectory(unit_tests)
add_subdirectory(integration_tests)

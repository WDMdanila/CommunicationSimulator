set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_DEBUG} -w -O0 -g3 -no-pie")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS_DEBUG} -w -O0 -g3 -no-pie")

if (CMAKE_COMPILER_IS_GNUCXX)
    include(${CMAKE_SOURCE_DIR}/cmake/CodeCoverage.cmake)
    append_coverage_compiler_flags()
endif ()

package_add_test(CommunicationIntegrationTests
        SOURCES CommunicationIntegrationTests.cpp
        )
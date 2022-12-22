include(CMakePrintHelpers)

function(execute_submodule)
    execute_process(COMMAND ${GIT_EXECUTABLE} submodule update ${ARGN} ./formatters
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            RESULT_VARIABLE GIT_SUBMOD_RESULT)
    message(Result of "git submodule update ${ARGN} -> ${GIT_SUBMOD_RESULT}")
endfunction()

function(update_submodules)
    find_package(Git QUIET)
    if (GIT_FOUND AND EXISTS "${CMAKE_SOURCE_DIR}/.git")
        # Update submodules as needed
        option(GIT_SUBMODULE "Check submodules during build" ON)
        if (GIT_SUBMODULE)
            message(STATUS "Submodule init")
            execute_submodule(--init --recursive)
            message(STATUS "Submodule update")
            execute_submodule(--remote --merge)
            execute_process(COMMAND ${GIT_EXECUTABLE} pull
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/formatters)
        endif ()
    endif ()
endfunction()

function(package_add_test TESTNAME)
    cmake_parse_arguments(ARGS "" "" "SOURCES;DEPENDS" ${ARGN})
    add_executable(${TESTNAME} ${ARGS_SOURCES})
    target_include_directories(${TESTNAME} PUBLIC ${CMAKE_SOURCE_DIR}/include)
    target_link_libraries(
            ${TESTNAME}
            ${CONAN_LIBS}
            ${CMAKE_DL_LIBS}
            ${ARGS_DEPENDS}
    )
    gtest_discover_tests(${TESTNAME}
            WORKING_DIRECTORY ${PROJECT_DIR}
            PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_DIR}"
            )
    set_target_properties(${TESTNAME} PROPERTIES FOLDER ${CMAKE_SOURCE_DIR}/tests)
endfunction()

function(add_app APP_NAME)
    add_executable(${APP_NAME} ${ARGN})
    get_property(PROJECT_LIBS GLOBAL PROPERTY PROJECT_LIBS_PROPERTY)
    target_link_libraries(${APP_NAME} PRIVATE "${PROJECT_LIBS}")
    set_link_options(${APP_NAME})
endfunction()

function(add_lib LIB_NAME)
    cmake_parse_arguments(ARGS "" "" "SOURCES;DEPENDS" ${ARGN})
    add_library(${LIB_NAME} ${ARGS_SOURCES})
    get_property(PROJECT_LIBS GLOBAL PROPERTY PROJECT_LIBS_PROPERTY)
    set_property(GLOBAL PROPERTY PROJECT_LIBS_PROPERTY "${PROJECT_LIBS};${LIB_NAME}")
    target_link_libraries(${LIB_NAME} PRIVATE "${ARGS_DEPENDS}")
    set_link_options(${LIB_NAME})
endfunction()

function(set_link_options TARGET_NAME)
    target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/include)
    target_link_libraries(${TARGET_NAME} PRIVATE ${CONAN_LIBS})
    target_compile_options(${TARGET_NAME} PRIVATE
            -Wno-unused-variable
            -Wno-maybe-uninitialized
            -Werror
            -Wall
            -Wextra
            -Wnon-virtual-dtor
            -Wcast-align
            -Wunused
            -Wpedantic
            -Wconversion
            -Wsign-conversion
            -Wmisleading-indentation
            -Wduplicated-cond
            -Wduplicated-branches
            -Wlogical-op
            -Wnull-dereference
            -Wuseless-cast
            -Wdouble-promotion
            )
    target_link_options(${TARGET_NAME} PRIVATE -rdynamic)
endfunction()

macro(setup_conan)
    download_conan_cmake()

    include(${CMAKE_BINARY_DIR}/conan/conan.cmake)
    set(CONAN_SYSTEM_INCLUDES ON)

    conan_cmake_run(
            CONANFILE ${CMAKE_SOURCE_DIR}/conanfile.txt
            PROFILE default
            BASIC_SETUP
            BUILD missing
    )
endmacro()

function(download_conan_cmake)
    if (NOT EXISTS "${CMAKE_BINARY_DIR}/conan/conan.cmake")
        message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
        file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/master/conan.cmake"
                "${CMAKE_BINARY_DIR}/conan/conan.cmake")
    endif ()
endfunction()

macro(setup_code_coverage)
    download_code_coverage_cmake()

    include(${CMAKE_BINARY_DIR}/cmake/CodeCoverage.cmake)
endmacro()

function(download_code_coverage_cmake)
    if (NOT EXISTS "${CMAKE_BINARY_DIR}/cmake/CodeCoverage.cmake")
        message(STATUS "Downloading CodeCoverage.cmake from https://github.com/bilke/cmake-modules/blob/master/CodeCoverage.cmake")
        file(DOWNLOAD "https://raw.githubusercontent.com/bilke/cmake-modules/master/CodeCoverage.cmake"
                "${CMAKE_BINARY_DIR}/cmake/CodeCoverage.cmake")
    endif ()
endfunction()

cmake_minimum_required(VERSION 3.20)

project(ginput VERSION 0.1.0 LANGUAGES CXX)

option(GINPUT_STRICT "Enable strict compiler warnings" ON)
option(GINPUT_WARN_AS_ERROR "Treat warnings as errors" OFF)
option(GINPUT_BUILD_EXAMPLES "Build example programs" ON)
option(GINPUT_BUILD_TESTS "Build test programs" ON)

add_library(ginput
    src/ginput.cpp
)

add_library(ginput::ginput ALIAS ginput)

target_include_directories(ginput
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_compile_features(ginput PUBLIC cxx_std_20)

function(ginput_apply_warnings target_name)
    if(NOT GINPUT_STRICT)
        return()
    endif()

    if(MSVC)
        target_compile_options(${target_name} PRIVATE /W4)
        if(GINPUT_WARN_AS_ERROR)
            target_compile_options(${target_name} PRIVATE /WX)
        endif()
    else()
        target_compile_options(${target_name} PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            -Wconversion
            -Wshadow
        )
        if(GINPUT_WARN_AS_ERROR)
            target_compile_options(${target_name} PRIVATE -Werror)
        endif()
    endif()
endfunction()

ginput_apply_warnings(ginput)

if(GINPUT_BUILD_EXAMPLES)
    add_executable(ginput_demo examples/ginput_demo.cpp)
    target_link_libraries(ginput_demo PRIVATE ginput::ginput)
    target_compile_features(ginput_demo PRIVATE cxx_std_20)
    ginput_apply_warnings(ginput_demo)
endif()

if(GINPUT_BUILD_TESTS)
    enable_testing()

    add_executable(ginput_tests tests/ginput_tests.cpp)
    target_link_libraries(ginput_tests PRIVATE ginput::ginput)
    target_compile_features(ginput_tests PRIVATE cxx_std_20)
    ginput_apply_warnings(ginput_tests)

    add_test(NAME ginput_tests COMMAND ginput_tests)
endif()


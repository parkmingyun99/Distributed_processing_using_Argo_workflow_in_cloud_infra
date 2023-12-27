cmake_minimum_required(VERSION 3.13.0)
project(SimRunner VERSION 1.0.0 DESCRIPTION "A basic header only math library" LANGUAGES CXX)
set(CMAKE_DEBUG_POSTFIX d)
add_executable(SimRunner)

target_compile_definitions(${PROJECT_NAME} PUBLIC -DAL_CONV_BUILD)
target_compile_features(SimRunner PUBLIC cxx_std_23)
set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "/INCREMENTAL:YES")
set_target_properties(
        SimRunner PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")

set_target_properties( SimRunner
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

target_link_libraries(SimRunner
        PRIVATE
        Engine
        Shop
        spdlog::spdlog_header_only
        )

target_include_directories(SimRunner
        PUBLIC
        ${CMAKE_SOURCE_DIR}/date/include
        ${CMAKE_SOURCE_DIR}/spdlog/include
        ${CMAKE_SOURCE_DIR}/Engine/
        ${CMAKE_SOURCE_DIR}/Shop/
        ${CMAKE_SOURCE_DIR}/json/single_include
        )

#if (WIN32)
#    add_definitions(-DDIR_EXPORT_IGNORE=1)
#    MESSAGE(STATUS "DIR_EXPORT_IGNORED")
#endif ()

file(GLOB SRC_FILES
        "*.cpp"
        )

target_sources(SimRunner PUBLIC
        ${SRC_FILES}
        )

#if (WIN32)
#    #    set_tests_properties(ex_cwn1 PROPERTIES ENVIRONMENT "PATH=${CMAKE_BINARY_DIR}/RNN")
#    #    message(STATUS "DLL paths: ${CMAKE_BINARY_DIR}/bin")
#    #    set_target_properties(SimRunner PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
#    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
#        add_custom_command(TARGET SimRunner
#                POST_BUILD
#                COMMAND ${CMAKE_COMMAND} -E copy_if_different SimRunner.exe ${CMAKE_BINARY_DIR}/bin
#                )
#    else()
#        add_custom_command(TARGET SimRunner
#                POST_BUILD
#                COMMAND ${CMAKE_COMMAND} -E copy_if_different SimRunner.exe ${CMAKE_BINARY_DIR}/bin
#                )
#    endif()
#
#endif ()

#add_custom_target(sim_exec_copy
#    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/Inputs ${CMAKE_BINARY_DIR}/bin
#)

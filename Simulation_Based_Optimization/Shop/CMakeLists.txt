cmake_minimum_required(VERSION 3.13.0)
project(Shop VERSION 1.0.0 DESCRIPTION "A basic header only math library" LANGUAGES CXX)
set(CMAKE_DEBUG_POSTFIX d)
add_library(Shop SHARED)

target_compile_definitions(${PROJECT_NAME} PUBLIC -DSM_MODEL_BUILD)
target_compile_features(Shop PUBLIC cxx_std_23)

target_link_libraries(Shop
        PRIVATE
        Engine
        spdlog::spdlog_header_only
        )

target_include_directories(Shop
        PUBLIC
        ${CMAKE_SOURCE_DIR}/spdlog/include
        ${CMAKE_SOURCE_DIR}/Engine
        )

#if (WIN32)
#    add_definitions(-DDIR_EXPORT_IGNORE=1)
#    MESSAGE(STATUS "DIR_EXPORT_IGNORED")
#endif ()

file(GLOB SRC_FILES
        "*.cpp"
        )

target_sources(Shop PUBLIC
        ${SRC_FILES}
        )

if (WIN32)
    #    set_tests_properties(ex_cwn1 PROPERTIES ENVIRONMENT "PATH=${CMAKE_BINARY_DIR}/RNN")
    #    message(STATUS "DLL paths: ${CMAKE_BINARY_DIR}/bin")
    #    set_target_properties(Shop PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_custom_command(TARGET Shop
                POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different Shopd.dll Shopd.lib ${CMAKE_BINARY_DIR}/bin
                )
    else ()
        add_custom_command(TARGET Shop
                POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different Shop.dll Shop.lib ${CMAKE_BINARY_DIR}/bin
                )
    endif ()

endif ()

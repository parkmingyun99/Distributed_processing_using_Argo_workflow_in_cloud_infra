cmake_minimum_required(VERSION 3.13.0)
project(Engine VERSION 1.0.0 DESCRIPTION "A basic header only math library" LANGUAGES CXX)
set(CMAKE_DEBUG_POSTFIX d)
add_library(Engine SHARED)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

target_compile_definitions(${PROJECT_NAME} PUBLIC -DSM_SIM_BUILD)
target_compile_features(Engine PUBLIC cxx_std_23)


target_link_libraries(Engine
        PRIVATE
        spdlog::spdlog_header_only
        )

target_include_directories(Engine
        PUBLIC
        ${CMAKE_SOURCE_DIR}/date/include
        ${CMAKE_SOURCE_DIR}/spdlog/include
        )


file(GLOB SRC_FILES
        "*.cpp"
        )

target_sources(Engine PUBLIC
        ${SRC_FILES}
        )

if (WIN32)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_custom_command(TARGET Engine
                POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different Engined.dll Engined.lib ${CMAKE_BINARY_DIR}/bin
                )
    else()
        add_custom_command(TARGET Engine
                POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different Engine.dll Engine.lib ${CMAKE_BINARY_DIR}/bin
                )
    endif()

endif ()

cmake_minimum_required(VERSION 3.13.0)
project(MultiAgvScheduling VERSION 1.0.0 DESCRIPTION "A test to see if I can make a CMake Project" LANGUAGES CXX)

add_subdirectory(spdlog)

add_subdirectory(Engine)

add_subdirectory(Shop)

add_subdirectory(json)

add_subdirectory(SimRunner)

#add_subdirectory(JobGenTest)

add_subdirectory(ArgoPermGA)

#add_subdirectory(OptExec)

#add_subdirectory(SimExec)
cmake_minimum_required(VERSION 3.6)
project(ToplSkeleton DESCRIPTION "A skeleton for Topl")

add_subdirectory(Rasteron)

add_library(Topl STATIC core/Topl_Renderer_Drx12.cpp)

target_include_directories(Topl PUBLIC core helper)
target_link_libraries(Topl Rasteron)
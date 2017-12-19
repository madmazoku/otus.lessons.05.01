cmake_minimum_required(VERSION 3.2)

project(otus.lessons.05.01)

find_package(Boost COMPONENTS unit_test_framework REQUIRED)
find_package(Threads REQUIRED)
find_package(SDL2 REQUIRED)

add_subdirectory("otus.lessons.05.01")

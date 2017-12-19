add_executable(
    otus.lessons.05.01 
        "otus.lessons.05.01.cpp"
        "global.cpp"
)

set_target_properties(otus.lessons.05.01 PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED ON
    COMPILE_OPTIONS -Wpedantic -Wall -Wextra
)

target_link_libraries(otus.lessons.05.01 ${SDL2_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

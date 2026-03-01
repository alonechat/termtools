add_executable(demo demo.cpp)
target_link_libraries(demo termtools)

add_executable(colors colors_demo.cpp)
target_link_libraries(colors termtools)

add_executable(table table_demo.cpp)
target_link_libraries(table termtools)

add_executable(progress progress_demo.cpp)
target_link_libraries(progress termtools)

add_executable(logger logger_demo.cpp)
target_link_libraries(logger termtools)

add_executable(prompt prompt_demo.cpp)
target_link_libraries(prompt termtools)

add_executable(keyboard keyboard_demo.cpp)
target_link_libraries(keyboard termtools)

# add_executable(test_progress test_progress.cpp)
# target_link_libraries(test_progress termtools)

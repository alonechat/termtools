add_executable(test_color test_color.cpp)
target_link_libraries(test_color termtools)

add_executable(test_text test_text.cpp)
target_link_libraries(test_text termtools)

add_executable(test_table test_table.cpp)
target_link_libraries(test_table termtools)

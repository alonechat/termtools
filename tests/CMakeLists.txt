add_executable(test_color test_color.cpp)
target_link_libraries(test_color termtools)

add_executable(test_text test_text.cpp)
target_link_libraries(test_text termtools)

add_executable(test_table test_table.cpp)
target_link_libraries(test_table termtools)

add_executable(test_markdown test_markdown.cpp)
target_link_libraries(test_markdown termtools)

add_executable(test_console test_console.cpp)
target_link_libraries(test_console termtools)

add_executable(test_utf8 test_utf8.cpp)
target_link_libraries(test_utf8 termtools)

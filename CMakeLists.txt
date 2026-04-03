cmake_minimum_required(VERSION 3.14)
project(termtools VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(TERMTOOLS_BUILD_EXAMPLES "Build example programs" ON)
option(TERMTOOLS_BUILD_TESTS "Build tests" ON)
option(TERMTOOLS_INSTALL "Generate install target" ON)

set(TERMTOOLS_SOURCES
        termtools/core/formatting/color.cpp
        termtools/core/formatting/style.cpp
        termtools/core/formatting/text.cpp
        termtools/core/output/console.cpp
        termtools/components/table/table.cpp
        termtools/components/progress/progress.cpp
        termtools/components/logger/status.cpp
        termtools/components/logger/logger.cpp
        termtools/core/input/prompt.cpp
        termtools/core/input/keyboard.cpp
        termtools/components/panel/panel.cpp
        termtools/ui/columns/columns.cpp
        termtools/ui/group/group.cpp
        termtools/ui/heading/heading.cpp
        termtools/ui/codeblock/codeblock.cpp
        termtools/ui/list/list.cpp
        termtools/ui/quote/quote.cpp
        termtools/ui/divider/divider.cpp
        termtools/utils/utils.cpp
        termtools/core/internal/platform.cpp
        termtools/components/markdown/markdown.cpp
        pixel/backend/backend.cpp
)

add_library(termtools STATIC ${TERMTOOLS_SOURCES})
target_include_directories(termtools PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

if(WIN32)
    target_compile_definitions(termtools PRIVATE _WIN32_WINNT=0x0601)
    target_link_libraries(termtools PRIVATE ws2_32)
elseif(UNIX)
    target_compile_definitions(termtools PRIVATE _POSIX_C_SOURCE=200809L)
endif()

if(TERMTOOLS_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if(TERMTOOLS_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

if(TERMTOOLS_INSTALL)
    include(GNUInstallDirs)
    install(TARGETS termtools
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
    install(FILES termtools.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
endif()

set_target_properties(termtools PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
    OUTPUT_NAME termtools
)

if(CMAKE_EXPORT_COMPILE_COMMANDS)
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E create_symlink
            "${CMAKE_BINARY_DIR}/compile_commands.json"
            "${CMAKE_SOURCE_DIR}/compile_commands.json"
        ERROR_QUIET OUTPUT_QUIET
    )
endif()

cmake_minimum_required(VERSION 3.20)
project(SCChallenge1)

set(CMAKE_CXX_STANDARD 20)
set(CPR_BUILD_TESTS_SSL OFF)

add_executable(SCChallenge1 src/main.cpp src/HtmlParser.cpp)

if(WIN32)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
endif()

include(FetchContent)
FetchContent_Declare(
        cpr
        GIT_REPOSITORY https://github.com/libcpr/cpr.git
        GIT_TAG d7cec83e9480253bacf11ff6441e9d00c08c71bf)
        FetchContent_MakeAvailable(cpr)

target_link_libraries(SCChallenge1 PRIVATE cpr::cpr)
cmake_minimum_required(VERSION 3.5.0)
project(SCChallenge1 VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 23)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../SCChallenge1 ${CMAKE_CURRENT_BINARY_DIR}/SCChallenge1)

add_library(SCChallenge1_IAT SHARED src/DllMain.cpp src/IAT.cpp src/SpaceCadets.cpp)
add_executable(SCChallengeInjector src/injector/main.cpp)


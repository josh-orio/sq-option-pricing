cmake_minimum_required(VERSION 3.28.0)
project(option-pricing VERSION 0.1.0 LANGUAGES C CXX)
set (CMAKE_CXX_STANDARD 20)

include(CTest)
enable_testing()

# DECLARE EXECUTABLE
add_executable(option-pricer main.cpp)

# LINK FMTLIB
find_package(fmt REQUIRED)
target_link_libraries(option-pricer fmt::fmt)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
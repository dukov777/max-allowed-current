cmake_minimum_required(VERSION 3.22)

# Project name
project(CapacitorTests)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set the build type to Debug
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

# Add the source files
set(SOURCES
    capacitors.cpp  # Assuming your class implementations are in this file
    tests.cpp       # The file containing your test cases
)

# Add GoogleTest manually
add_subdirectory(googletest)

# Add the executable for tests
add_executable(CapacitorTests ${SOURCES})

# Link GoogleTest with your test executable
target_link_libraries(CapacitorTests gtest_main)

# Enable testing
enable_testing()

# Add tests
add_test(NAME CapacitorTests COMMAND CapacitorTests)

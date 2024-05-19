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

cmake_minimum_required(VERSION 3.14)
project(my_project)

# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  # URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Add the source files
set(SOURCES
    capacitors.cpp  # Assuming your class implementations are in this file
    tests.cpp       # The file containing your test cases
)

# # Add GoogleTest manually
# add_subdirectory(googletest)

# Add the executable for tests
add_executable(CapacitorTests ${SOURCES})

# Link GoogleTest with your test executable
target_link_libraries(CapacitorTests gtest_main)

# Enable testing
enable_testing()

# Add tests
add_test(NAME CapacitorTests COMMAND CapacitorTests)

include(GoogleTest)
gtest_discover_tests(CapacitorTests)
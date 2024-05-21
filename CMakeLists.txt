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
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Add the source files
set(SOURCES
    src/capacitors.cpp  # Assuming your class implementations are in this file
    src/capacitor_tank.cpp
    src/capacitor_violation_check.cpp
)

set(TEST_SOURCES
  ${SOURCES}
  tests/tests.cpp       # The file containing your test cases
  tests/test_capacitor_tank.cpp
)

set(APP_SOURCES
  ${SOURCES}
  main.cpp
)

include_directories(${CMAKE_SOURCE_DIR}/include/)
include_directories(${CMAKE_SOURCE_DIR}/argparse/include/)
include_directories(${CMAKE_SOURCE_DIR}/json/include/)

# Add -fsanitize=address flag
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
# set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -fsanitize=address")
# set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -fsanitize=address")

enable_testing()

add_executable(
  calculator
  ${APP_SOURCES}
)

add_executable(
  CapacitorTests
  ${TEST_SOURCES}
)

target_link_libraries(
  CapacitorTests
  GTest::gtest_main
)

target_compile_options(calculator PRIVATE -DLOG_CONSOLE)

include(GoogleTest)

gtest_discover_tests(CapacitorTests)

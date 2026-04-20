cmake_minimum_required(VERSION 3.14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall" CACHE INTERNAL "")
project(PrototypeExamQuestionGenerator CXX)

# Binaries go to ./build/ so graders find begin_exam_generator predictably.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)

add_library(exam_generator
  src/option_parser_strategy.cpp
  src/draft_question_parser.cpp
  src/latex_renderer.cpp
  src/exam_question_generator.cpp
)
target_include_directories(exam_generator PUBLIC ${CMAKE_SOURCE_DIR}/include)

add_executable(begin_exam_generator src/main.cpp)
target_link_libraries(begin_exam_generator PRIVATE exam_generator)

include(CTest)
enable_testing()
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(test_exam_generator tests/test_exam_generator.cpp)
target_link_libraries(test_exam_generator
  PRIVATE
  exam_generator
  GTest::gtest_main
)
target_compile_definitions(test_exam_generator PRIVATE PROJECT_SOURCE_DIR="${CMAKE_SOURCE_DIR}")

# add_test only (no gtest_discover_tests): avoids running tests at cmake configure time.
add_test(
  NAME exam_generator_tests
  COMMAND test_exam_generator
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

# put test files in different directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/test/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/test/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/test/bin)

include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/test)
link_directories(${CMAKE_BINARY_DIR}/lib)
add_library(gtest gtest-all.cc)
add_library(gmock gmock-gtest-all.cc)
add_executable(unittest main.cpp snap_unittest.cpp)
target_link_libraries(unittest pthread gtest gmock snap)

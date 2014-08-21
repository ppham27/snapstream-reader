add_library(snap snap.cpp)
add_library(Program Program.cpp)
target_link_libraries(snap Program)

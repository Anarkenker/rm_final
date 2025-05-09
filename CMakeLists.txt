cmake_minimum_required(VERSION 3.10)
project(part2)

set(CMAKE_CXX_STANDARD 17)

add_executable(read_fc
    code/part2/read_fc.cpp
    code/part1/scr/Matrix.cpp
    code/part1/scr/model.cpp
)

target_include_directories(read_fc PRIVATE code/part1/include)

find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries(read_fc PRIVATE nlohmann_json::nlohmann_json)
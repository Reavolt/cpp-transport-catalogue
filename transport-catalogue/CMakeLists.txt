cmake_minimum_required(VERSION 3.16.0)
project(ya_cpp VERSION 0.1.0)

add_executable(${PROJECT_NAME} 
    main.cpp
    domain.cpp
    geo.cpp
    input_reader.cpp
    json.cpp
    main.cpp
    stat_reader.cpp
    svg.cpp
    transport_catalogue.cpp
    json_reader.cpp
    map_renderer.cpp
)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_GLIBCXX_DEBUG")
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
target_link_libraries(${PROJECT_NAME} PUBLIC -ltbb -pthread)
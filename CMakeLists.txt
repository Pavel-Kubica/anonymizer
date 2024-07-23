cmake_minimum_required(VERSION 3.22.1)
project(anonymizer)

set(CMAKE_CXX_STANDARD 20)

file(GLOB_RECURSE SOURCE
        src/*
)
include_directories(src)
include_directories(src/database)
include_directories(src/message-pipeline)
include_directories(src/util)

add_executable(anonymizer ${SOURCE} src/main.cpp)
target_compile_options(anonymizer PRIVATE -Wall -pedantic -g)
target_link_libraries(anonymizer PRIVATE kj capnp rdkafka++)

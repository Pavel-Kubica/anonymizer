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
include_directories(src/application)
include(FetchContent)
FetchContent_Declare(cpr GIT_REPOSITORY https://github.com/libcpr/cpr.git
        GIT_TAG 3b15fa82ea74739b574d705fea44959b58142eb8)
FetchContent_MakeAvailable(cpr)

add_executable(anonymizer ${SOURCE} src/main.cpp)
target_compile_options(anonymizer PRIVATE -Wall -pedantic -g)
target_link_libraries(anonymizer PRIVATE kj capnp rdkafka++ cpr::cpr)

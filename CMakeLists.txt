cmake_minimum_required(VERSION 3.13)
project(time_interval_sum_server)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)

include_directories(CppServer/modules)
include_directories(CppServer/modules/asio/asio/include)
include_directories(CppServer/modules/CppCommon/include)
include_directories(CppServer/modules/cpp-optparse)
include_directories(CppServer/include)

link_directories(CppServer/temp)
link_directories(CppServer/temp/modules)
link_directories(CppServer/temp/modules/CppCommon)
link_directories(CppServer/temp/modules/CppCommon/modules)

find_package(Threads REQUIRED)
link_libraries(Threads::Threads)

add_compile_options(-Wall -pedantic -Werror)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-O0 -g3)
else()
    add_compile_options(-Ofast -mavx -march=native -flto -g0 -DNDEBUG)
endif()

add_subdirectory(test)
add_subdirectory(server)


cmake_minimum_required(VERSION 3.13)
project(time_interval_sum_server)

set(CMAKE_CXX_STANDARD 17)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(CppServer/modules)
include_directories(CppServer/modules/asio/asio/include)
include_directories(CppServer/modules/CppCommon/include)
include_directories(CppServer/modules/cpp-optparse)
include_directories(CppServer/include)

link_directories(CppServer/temp)
link_directories(CppServer/temp/modules)
link_directories(CppServer/temp/modules/CppCommon)
link_directories(CppServer/temp/modules/CppCommon/modules)

add_executable(time_interval_sum_server
 main.cpp
)

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

link_libraries(Threads::Threads)
target_link_libraries(time_interval_sum_server
 cppserver
 cppcommon
 fmt
 asio
 cpp-optparse
 dl
 uuid
 ssl
 crypto
 Threads::Threads
)

add_subdirectory(test)

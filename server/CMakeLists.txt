add_executable(time_interval_sum_server
    src/main.cpp
)

include_directories(include)

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
)
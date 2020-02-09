project(time_interval_sum_server_test)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../server/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../googletest/googletest/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../googletest/googlemock/include)

link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../googletest/build/lib/)

add_executable(time_interval_sum_server_test
        parser_test.cpp
        calculator_test.cpp
)

target_link_libraries(time_interval_sum_server_test
    gtest_main
    gtest
    gmock_main
    gmock
)
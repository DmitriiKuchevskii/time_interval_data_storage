To build:
0. Install gil:
    pip3 install gil

1. Download CppServer library by executing:
    sh setup.sh

2. Build the server by executing the following:
    mkdir build
    cd build
    cmake --build-64bit -DCMAKE_BUILD_TYPE=Release ..
    make


To run the server (it uses port 1111):
    ./time_interval_sum_server

Now you can connect and send yor numbers.
PLEASE NOTE: There are no format checks!!! Incorrect format will lead to UB.

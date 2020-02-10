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


To run the server:
    ./server/time_interval_sum_server
Options:
  --version             show program's version number and exit
  -h, --help            show this help message and exit
  -p PORT, --port=PORT  Server port. Default: 1111
  -t TIME, --time=TIME  Time interval for sum in milliseconds. Default: 60000
  -i TYPE, --inputtype=TYPE
                        Input numbers type. Default: integer
  
  
To run tests:
    ./test/time_interval_sum_server_test

Now you can connect and send yor numbers.

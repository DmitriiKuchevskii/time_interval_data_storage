git clone https://github.com/chronoxor/CppServer.git  || exit
cd CppServer                                          || exit
gil update                                            || exit
cd build                                              || exit
sh unix.sh                                            || exit
cd ../../                                             || exit

git clone https://github.com/google/googletest.git     || exit
cd googletest                                          || exit
mkdir build                                            || exit
cd build                                               || exit
cmake ..                                               || exit
make -j                                                || exit
cd ../../                                              || exit

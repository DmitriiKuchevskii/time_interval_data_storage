git clone https://github.com/chronoxor/CppServer.git  || exit
cd CppServer                                          || exit
gil update                                            || exit
cd build                                              || exit
sed -i '6s/.*//' ./unix.sh                            || exit
sed -i '6s/.*/cmake --build-64bit -DCMAKE_BUILD_TYPE=Release CXXFLAGS="-Ofast -mavx -m64 -march=native -flto -g0 -DNDEBUG" -DCMAKE_INSTALL_PREFIX=..\/temp\/install -G "Unix Makefiles" ../' ./Unix/01-generate.sh || exit
sh unix.sh                                            || exit
cd ../../                                             || exit

git clone https://github.com/google/googletest.git     || exit
cd googletest                                          || exit
mkdir build                                            || exit
cd build                                               || exit
cmake --build-64bit -DCMAKE_BUILD_TYPE=Release CXXFLAGS="-Ofast -mavx -m64 -march=native -flto -g0 -DNDEBUG" .. || exit
make -j                                                || exit
cd ../../                                              || exit

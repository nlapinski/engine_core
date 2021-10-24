cp third-party/build/libtcc-prefix/src/libtcc/include/tcc/libtcc.h third-party/build/include/libtcc.h
rm  ./build/release-ninja/engine_core.exe
cmake --build build/release-ninja/

mkdir build/debug-ninja/include
mkdir build/debug-ninja/lib
mkdir build/release-ninja/include
mkdir build/release-ninja/lib

cp third-party/build/libtcc-prefix/src/libtcc/src/tcclib.h build/debug-ninja/include/tcclib.h
cp third-party/build/libtcc-prefix/src/libtcc/src/tcclib.h build/release-ninja/include/tcclib.h
cp -r third-party/build/include_tinycc/* build/release-ninja/include
cp -r third-party/build/include_tinycc/* build/debug-ninja/include
cp third-party/build/lib/libtcc_x86_64.a build/release-ninja/lib/libtcc1-x86_64.a
cp third-party/build/lib/libtcc_x86_64.a build/debug-ninja/lib/libtcc1-x86_64.a

./build/release-ninja/engine_core.exe

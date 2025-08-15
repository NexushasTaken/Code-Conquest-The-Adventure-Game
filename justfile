configure-linux:
  cmake -B build-linux-cmake -DPLATFORM=Desktop --fresh \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_C_COMPILER_LAUNCHER=sccache -DCMAKE_CXX_COMPILER_LAUNCHER=sccache \
    -DCMAKE_EXE_LINKER_FLAGS="-fuse-ld=mold" \
    -DCMAKE_C_COMPILER=clang \
    -DCMAKE_CXX_COMPILER=clang++
  ln -sf build-linux-cmake/compile_commands.json compile_commands.json

configure-android:
  cmake -B build-android-cmake -DPLATFORM=Android --fresh \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_C_COMPILER_LAUNCHER=sccache -DCMAKE_CXX_COMPILER_LAUNCHER=sccache \
    -DCMAKE_EXE_LINKER_FLAGS="-fuse-ld=mold"
  ln -sf build-android-cmake/compile_commands.json compile_commands.json

build-linux-cmake:
  cmake --build build-linux-cmake --parallel

build-android-cmake:
  cmake --build build-android-cmake --parallel

build-android-make:
  make TARGET=Android

install-android-make:
  make TARGET=Android install

run-android-make:
  make TARGET=Android run

build-linux-make:
  make TARGET=Linux -j

run-linux-cmake: build-linux-cmake
  ./build-linux-cmake/main

run-linux-make: build-linux-make
  ./build-linux/game

debug-linux-cmake: build-linux-cmake
  gdb ./build-linux-cmake/main

debug-linux-make: build-linux-make
  gdb ./build-linux/game

clean:
  make TARGET=Android clean
  make clean
  rm -rf build build-android-cmake build-linux

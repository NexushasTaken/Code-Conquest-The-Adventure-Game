configure-linux:
  cmake -B build-linux -DPLATFORM=Desktop --fresh \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_C_COMPILER_LAUNCHER=sccache -DCMAKE_CXX_COMPILER_LAUNCHER=sccache \
    -DCMAKE_EXE_LINKER_FLAGS="-fuse-ld=mold"
  ln -sf build-linux/compile_commands.json compile_commands.json

configure-android:
  cmake -B build-android -DPLATFORM=Android --fresh \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_C_COMPILER_LAUNCHER=sccache -DCMAKE_CXX_COMPILER_LAUNCHER=sccache \
    -DCMAKE_EXE_LINKER_FLAGS="-fuse-ld=mold"
  ln -sf build-android/compile_commands.json compile_commands.json

build-linux:
  cmake --build build-linux --parallel

build-android:
  cmake --build build-android --parallel

build-android-make:
  make TARGET=Android

install-android:
  make TARGET=Android install

build-linux-make:
  make TARGET=Linux -j

run-linux: build-linux
  ./build-linux/main

debug-linux: build-linux
  gdb ./build-linux/main

clean:
  make TARGET=Android clean
  make clean
  rm -rf build build-android build-linux

configure-linux:
  cmake -B build-linux -DTARGET=Linux --fresh
  ln -sf build-linux/compile_commands.json compile_commands.json

#-DCMAKE_C_COMPILER_LAUNCHER=sccache -DCMAKE_CXX_COMPILER_LAUNCHER=sccache
#-DCMAKE_C_COMPILER_LAUNCHER=sccache -DCMAKE_CXX_COMPILER_LAUNCHER=sccache
configure-android:
  cmake -B build-android -DTARGET=Android --fresh
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

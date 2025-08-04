#!/bin/sh
# ______________________________________________________________________________
#
#  Compile raylib project for Android
# ______________________________________________________________________________

# stop on error and display each command as it gets executed. Optional step but helpful in catching where errors happen if they do.
set -xe

# NOTE: If you excluded any ABIs in the previous steps, remove them from this list too
ABIS="arm64-v8a armeabi-v7a x86 x86_64"
API_VERSION="31"

BUILD=$(pwd)/android/build
NAME=game
DEV_NAME=raylib
SRC=src/*.cpp
TARGET=Android

SDK=$(pwd)/android/sdk/$(uname)
NDK=$(pwd)/android/ndk/$(uname)/android-ndk-r25b
JAVA=/usr/lib/jvm/java-21-openjdk

BUILD_TOOLS=$SDK/build-tools/36.0.0/
TOOLCHAIN=$NDK/toolchains/llvm/prebuilt/linux-x86_64
NATIVE_APP_GLUE=$NDK/sources/android/native_app_glue

AR=$TOOLCHAIN/bin/llvm-ar

FLAGS="-ffunction-sections -funwind-tables -fstack-protector-strong -fPIC -Wall \
  -Wa,--noexecstack -Wformat -Werror=format-security -no-canonical-prefixes \
  -DANDROID -DPLATFORM_ANDROID -D__ANDROID_API__=$API_VERSION"

INCLUDES="-I. -Iinclude -I../include -I$NATIVE_APP_GLUE -I$TOOLCHAIN/sysroot/usr/include"

# Copy icons
cp assets/icon_ldpi.png $BUILD/res/drawable-ldpi/icon.png
cp assets/icon_mdpi.png $BUILD/res/drawable-mdpi/icon.png
cp assets/icon_hdpi.png $BUILD/res/drawable-hdpi/icon.png
cp assets/icon_xhdpi.png $BUILD/res/drawable-xhdpi/icon.png

# Copy other assets
cp assets/* $BUILD/assets

# ______________________________________________________________________________
#
#  Compile
# ______________________________________________________________________________

for ABI in $ABIS; do
  case "$ABI" in
    "armeabi-v7a")
      CCTYPE="armv7a-linux-androideabi"
      ARCH="arm"
      ARCH_ALT="$ARCH"
      LIBPATH="arm-linux-androideabi"
      ABI_FLAGS="-march=armv7-a -mfloat-abi=softfp -mfpu=vfpv3-d16"
      ;;

    "arm64-v8a")
      CCTYPE="aarch64-linux-android"
      ARCH="aarch64"
      ARCH_ALT="arm64"
      LIBPATH="aarch64-linux-android"
      ABI_FLAGS="-target aarch64 -mfix-cortex-a53-835769"
      ;;

    "x86")
      CCTYPE="i686-linux-android"
      ARCH="i386"
      ARCH_ALT="x86"
      LIBPATH="i686-linux-android"
      ABI_FLAGS=""
      ;;

    "x86_64")
      CCTYPE="x86_64-linux-android"
      ARCH="x86_64"
      ARCH_ALT="$ARCH"
      LIBPATH="x86_64-linux-android"
      ABI_FLAGS=""
      ;;
  esac
  CC="$TOOLCHAIN/bin/$CCTYPE$API_VERSION-clang"
  CXX="$TOOLCHAIN/bin/$CCTYPE$API_VERSION-clang++"

  if [[ ! -f "lib/Android/$ABI/libraylib.a" ]]; then
    make -C externals/raylib/src clean
    make -C externals/raylib/src PLATFORM=PLATFORM_ANDROID ANDROID_NDK=$NDK ANDROID_ARCH=$ARCH_ALT ANDROID_API_VERSION=$API_VERSION RAYLIB_BUILD_MODE=RELEASE -j
    make -C externals/raylib/src clean
    mkdir -p lib/Android/$ABI/
    mv externals/raylib/src/libraylib.a lib/$TARGET/$ABI/
  fi

  # Compile native app glue
  # .c -> .o
  $CC -c $NATIVE_APP_GLUE/android_native_app_glue.c -o $NATIVE_APP_GLUE/native_app_glue.o \
    $INCLUDES -I$TOOLCHAIN/sysroot/usr/include/$CCTYPE $FLAGS $ABI_FLAGS

  # .o -> .a
  $AR rcs lib/$TARGET/$ABI/libnative_app_glue.a $NATIVE_APP_GLUE/native_app_glue.o

  # Compile project
  # FLAGS and TYPEFLAGS are from the main build script which sources this one
  mkdir -p $BUILD/lib/$ABI
  $CXX $SRC -o $BUILD/lib/$ABI/libmain.so -shared \
  -Wl,--exclude-libs,libatomic.a \
  -Wl,--build-id \
  -Wl,-z,noexecstack \
  -Wl,-z,relro \
  -Wl,-z,now \
  -Wl,--warn-shared-textrel \
  -Wl,--fatal-warnings \
  -u ANativeActivity_onCreate \
  "-L$TOOLCHAIN/sysroot/usr/lib/$LIBPATH/$API_VERSION" \
  -L$TOOLCHAIN/lib/clang/17/lib/linux/$ARCH \
  -L. -L$BUILD/obj -Llib/$TARGET/$ABI \
  -lraylib -lnative_app_glue -llog -landroid -lEGL -lGLESv2 -lOpenSLES -lc -lm -ldl
done

# ______________________________________________________________________________
#
#  Build APK
# ______________________________________________________________________________
#
$BUILD_TOOLS/aapt package -f -m \
  -S $BUILD/res -J $BUILD/src -M $BUILD/AndroidManifest.xml \
  -I $SDK/platforms/android-$API_VERSION/android.jar

# Compile NativeLoader.java
$JAVA/bin/javac -verbose -source 1.8 -target 1.8 \
  -bootclasspath jre/lib/rt.jar \
  -classpath $SDK/platforms/android-$API_VERSION/android.jar \
  -sourcepath src $BUILD/src/com/$DEV_NAME/$NAME/R.java \
  -d $BUILD/classes \
  $BUILD/src/com/$DEV_NAME/$NAME/NativeLoader.java

$BUILD_TOOLS/d8 --classpath $SDK/platforms/android-$API_VERSION/android.jar --output $BUILD/dex $BUILD/classes/com/$DEV_NAME/$NAME/*.class

# Add resources and assets to APK
$BUILD_TOOLS/aapt package -f \
  -M $BUILD/AndroidManifest.xml -S $BUILD/res -A assets \
  -I $SDK/platforms/android-$API_VERSION/android.jar -F $NAME.apk $BUILD/dex

# Add libraries to APK
cd $BUILD
for ABI in $ABIS; do
  $BUILD_TOOLS/aapt add ../../$NAME.apk lib/$ABI/libmain.so
done
cd ../..

# 1. Zipalign the unsigned APK
$BUILD_TOOLS/zipalign -f 4 $NAME.apk $NAME.aligned.apk

# 2. Sign the aligned APK
$BUILD_TOOLS/apksigner sign \
  --ks android/$DEV_NAME.keystore \
  --ks-pass pass:$DEV_NAME \
  --out $NAME.signed.apk \
  $NAME.aligned.apk

# 3. Replace original
mv $NAME.signed.apk $NAME.apk


# Install to device or emulator if -r was specified
[[ "$1" = "-r" ]] && $SDK/platform-tools/adb install -r $NAME.apk || \builtin true

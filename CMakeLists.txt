cmake_minimum_required(VERSION 3.5)

set(SRC ./src/main.cpp)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(CMAKE_SYSTEM_NAME STREQUAL "Android")
  set(DEV_NAME raylib)
  set(NAME game)
  set(SDK ${CMAKE_CURRENT_SOURCE_DIR}/android/sdk/${CMAKE_HOST_SYSTEM_NAME})
  set(NDK ${CMAKE_CURRENT_SOURCE_DIR}/android/ndk/${CMAKE_HOST_SYSTEM_NAME}/android-ndk-r25b)
  set(BUILD_TOOLS_VERSION 36.0.0)
  set(BUILD_TOOLS ${CMAKE_CURRENT_SOURCE_DIR}/android/sdk/${CMAKE_HOST_SYSTEM_NAME}/build-tools/${BUILD_TOOLS_VERSION})

  set(CMAKE_SYSTEM_VERSION 31) # API level
  set(CMAKE_ANDROID_NDK ${NDK})
  set(ANDROID_NDK ${CMAKE_ANDROID_NDK})

  set(PLATFORM "Android" CACHE STRING "Target platform for raylib" FORCE)

  # add_library(native_app_glue ${CMAKE_ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)
  #target_include_directories(native_app_glue PUBLIC ${CMAKE_ANDROID_NDK}/sources/android/native_app_glue/)

  include(UseJava)
  project(Main CXX)

  add_library(main SHARED ${SRC})

  file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/src/com/${DEV_NAME}/${NAME}/)
  add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/src/com/${DEV_NAME}/${NAME}/R.java
    COMMAND ${BUILD_TOOLS}/aapt package -f -m
            -S ${CMAKE_CURRENT_SOURCE_DIR}/android/build/res
            -J ${CMAKE_CURRENT_BINARY_DIR}/src
            -M ${CMAKE_CURRENT_SOURCE_DIR}/android/build/AndroidManifest.xml
            -I ${SDK}/platforms/android-${CMAKE_SYSTEM_VERSION}/android.jar
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/android/build/AndroidManifest.xml
            ${CMAKE_CURRENT_SOURCE_DIR}/android/build/res/**/*
    COMMENT "Packaging resources and generating R.java using aapt"
  )

  add_custom_target(generate_resources
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/src/com/${DEV_NAME}/${NAME}/R.java
  )

  add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/src/com/${DEV_NAME}/${NAME}/NativeLoader.class
           ${CMAKE_CURRENT_BINARY_DIR}/src/com/${DEV_NAME}/${NAME}/R\$attr.class
           ${CMAKE_CURRENT_BINARY_DIR}/src/com/${DEV_NAME}/${NAME}/R\$drawable.class
           ${CMAKE_CURRENT_BINARY_DIR}/src/com/${DEV_NAME}/${NAME}/R.class
    COMMAND javac -verbose -source 1.8 -target 1.8
            -bootclasspath jre/lib/rt.jar
            -classpath ${SDK}/platforms/android-${CMAKE_SYSTEM_VERSION}/android.jar
            -sourcepath src ${CMAKE_CURRENT_SOURCE_DIR}/android/build/src/com/${DEV_NAME}/${NAME}/R.java
            -d ${CMAKE_CURRENT_BINARY_DIR}/build/class
            ${CMAKE_CURRENT_SOURCE_DIR}/android/build/src/com/${DEV_NAME}/${NAME}/NativeLoader.java
    DEPENDS generate_resources
            ${CMAKE_CURRENT_SOURCE_DIR}/android/build/src/com/${DEV_NAME}/${NAME}/R.java
    COMMENT "Packaging resources and generating R.java using aapt"
  )
  add_custom_target(nativeloader
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/src/com/${DEV_NAME}/${NAME}/NativeLoader.class
            ${CMAKE_CURRENT_BINARY_DIR}/src/com/${DEV_NAME}/${NAME}/R\$attr.class
            ${CMAKE_CURRENT_BINARY_DIR}/src/com/${DEV_NAME}/${NAME}/R\$drawable.class
            ${CMAKE_CURRENT_BINARY_DIR}/src/com/${DEV_NAME}/${NAME}/R.class
  )

  add_dependencies(main nativeloader)

  # Compile NativeLoader.java
  #$JAVA/bin/javac -verbose -source 1.8 -target 1.8 \
  #  -bootclasspath jre/lib/rt.jar \
  #  -classpath $SDK/platforms/android-$API_VERSION/android.jar \
  #  -sourcepath src $BUILD/src/com/$DEV_NAME/$NAME/R.java \
  #  -d $BUILD/classes \
  #  $BUILD/src/com/$DEV_NAME/$NAME/NativeLoader.java
endif()
add_subdirectory(./externals/raylib/)

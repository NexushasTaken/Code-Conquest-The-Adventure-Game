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

  set(MANIFEST ${CMAKE_CURRENT_SOURCE_DIR}/src/com/AndroidManifest.xml)
  set(ANDROID_JAR ${SDK}/platforms/android-${CMAKE_SYSTEM_VERSION}/android.jar)
  set(RES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/assets/res)
  set(RES ${RES_DIR}/**/*)
  set(R_JAVA ${CMAKE_CURRENT_BINARY_DIR}/src/com/${DEV_NAME}/${NAME}/R.java)

  file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/src/com/${DEV_NAME}/${NAME}/)
  add_custom_command(
    OUTPUT ${R_JAVA}
    COMMAND ${BUILD_TOOLS}/aapt package -f -m
            -S ${CMAKE_CURRENT_SOURCE_DIR}/assets/res
            -J ${CMAKE_CURRENT_BINARY_DIR}/src
            -M ${MANIFEST}
            -I ${ANDROID_JAR}
    DEPENDS ${MANIFEST} ${RES}
    COMMENT "Packaging resources and generating R.java using aapt"
  )

  add_custom_target(generate_resources DEPENDS ${R_JAVA})

  set(NATIVE_LOADER_OUT ${CMAKE_CURRENT_BINARY_DIR}/src/com/${DEV_NAME}/${NAME}/NativeLoader.class
                        ${CMAKE_CURRENT_BINARY_DIR}/src/com/${DEV_NAME}/${NAME}/R.class)
  add_custom_command(
    OUTPUT ${NATIVE_LOADER_OUT}
    COMMAND javac -verbose -source 1.8 -target 1.8
            -bootclasspath jre/lib/rt.jar
            -classpath ${ANDROID_JAR}
            -sourcepath src ${R_JAVA}
            -d ${CMAKE_CURRENT_BINARY_DIR}/src
            ${CMAKE_CURRENT_SOURCE_DIR}/android/build/src/com/${DEV_NAME}/${NAME}/NativeLoader.java
    DEPENDS generate_resources
    COMMENT "Compiling NativeLoader"
  )
  add_custom_target(nativeloader DEPENDS ${NATIVE_LOADER_OUT})

  file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/dex/)
  add_custom_command(
    OUTPUT dex/classes.dex
    COMMAND ${BUILD_TOOLS}/d8 --classpath ${ANDROID_JAR}
            --output ${CMAKE_CURRENT_BINARY_DIR}/dex/
            ${NATIVE_LOADER_OUT}
    DEPENDS nativeloader
    COMMENT "Generate classes.dex"
  )
  add_custom_target(classes_dex DEPENDS dex/classes.dex)

  add_dependencies(main classes_dex)
endif()
add_subdirectory(./externals/raylib/)

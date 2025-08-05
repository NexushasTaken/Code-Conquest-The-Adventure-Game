cmake_minimum_required(VERSION 3.12)

set(SRC ./src/main.cpp)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(CMAKE_SYSTEM_NAME STREQUAL "Android" AND PLATFORM STREQUAL "Android")
  set(SDK ${CMAKE_CURRENT_SOURCE_DIR}/android/sdk/${CMAKE_HOST_SYSTEM_NAME})
  set(NDK ${CMAKE_CURRENT_SOURCE_DIR}/android/ndk/${CMAKE_HOST_SYSTEM_NAME}/android-ndk-r25b)
  set(BUILD_TOOLS_VERSION 36.0.0)
  set(BUILD_TOOLS ${CMAKE_CURRENT_SOURCE_DIR}/android/sdk/${CMAKE_HOST_SYSTEM_NAME}/build-tools/${BUILD_TOOLS_VERSION})

  set(CMAKE_SYSTEM_VERSION 31) # API level
  set(CMAKE_ANDROID_NDK ${NDK})
  set(ANDROID_NDK ${CMAKE_ANDROID_NDK})
  set(CMAKE_ANDROID_ARCH_ABI armeabi-v7a)

  add_library(native_app_glue ${CMAKE_ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)
  target_include_directories(native_app_glue PUBLIC ${CMAKE_ANDROID_NDK}/sources/android/native_app_glue/)

  set(CURL_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/externals/curl/include/)
  if(CMAKE_ANDROID_ARCH_ABI STREQUAL "armeabi-v7a")
    set(CURL_LIBRARIES ${CMAKE_CURRENT_SOURCE_DIR}/externals/curl/armeabi-v7a/libcurl.a)
  elseif(CMAKE_ANDROID_ARCH_ABI STREQUAL "arm64-v8a")
    set(CURL_LIBRARIES ${CMAKE_CURRENT_SOURCE_DIR}/externals/curl/arm64-v8a/libcurl.a)
  elseif(CMAKE_ANDROID_ARCH_ABI STREQUAL "x86")
    set(CURL_LIBRARIES ${CMAKE_CURRENT_SOURCE_DIR}/externals/curl/x86/libcurl.a)
  elseif(CMAKE_ANDROID_ARCH_ABI STREQUAL "x86_64")
    set(CURL_LIBRARIES ${CMAKE_CURRENT_SOURCE_DIR}/externals/curl/x86_64/libcurl.a)
  else()
    set(CURL_LIBRARIES "")
  endif()

  set(ZLIB_LIBRARIES "-lz")

else()
  find_package(PkgConfig REQUIRED)
  pkg_check_modules(CURL REQUIRED libcurl)
  pkg_check_modules(ZLIB REQUIRED zlib)

endif()

project(Main)

add_library(main SHARED ./src/main.cpp)

target_link_libraries(main ${CURL_LIBRARIES})
target_include_directories(main PUBLIC ${CURL_INCLUDE_DIRS})
target_compile_options(main PUBLIC ${CUR_CFLAGS_OTHER})

target_link_libraries(main ${ZLIB_LIBRARIES})
target_include_directories(main PUBLIC ${ZLIB_INCLUDE_DIRS})
target_compile_options(main PUBLIC ${ZLIB_CFLAGS_OTHER})

add_subdirectory(./externals/raylib/)
target_link_libraries(main raylib)

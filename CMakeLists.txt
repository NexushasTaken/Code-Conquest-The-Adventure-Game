cmake_minimum_required(VERSION 3.12)

set(SRCS
  ./src/main.cpp
)

list(APPEND SRCS ./externals/lua/src/lapi.c ./externals/lua/src/lcode.c ./externals/lua/src/lctype.c ./externals/lua/src/ldebug.c ./externals/lua/src/ldo.c ./externals/lua/src/ldump.c ./externals/lua/src/lfunc.c ./externals/lua/src/lgc.c ./externals/lua/src/llex.c ./externals/lua/src/lmem.c ./externals/lua/src/lobject.c ./externals/lua/src/lopcodes.c ./externals/lua/src/lparser.c ./externals/lua/src/lstate.c ./externals/lua/src/lstring.c ./externals/lua/src/ltable.c ./externals/lua/src/ltm.c ./externals/lua/src/lundump.c ./externals/lua/src/lvm.c ./externals/lua/src/lzio.c ./externals/lua/src/lauxlib.c ./externals/lua/src/lbaselib.c ./externals/lua/src/lcorolib.c ./externals/lua/src/ldblib.c ./externals/lua/src/liolib.c ./externals/lua/src/lmathlib.c ./externals/lua/src/loadlib.c ./externals/lua/src/loslib.c ./externals/lua/src/lstrlib.c ./externals/lua/src/ltablib.c ./externals/lua/src/lutf8lib.c ./externals/lua/src/linit.c)
add_compile_options(-DLUA_COMPAT_5_3)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(PLATFORM STREQUAL "Android")
  set(SDK ${CMAKE_CURRENT_SOURCE_DIR}/android/sdk/${CMAKE_HOST_SYSTEM_NAME})
  set(NDK ${CMAKE_CURRENT_SOURCE_DIR}/android/ndk/${CMAKE_HOST_SYSTEM_NAME}/android-ndk-r25b)
  set(BUILD_TOOLS_VERSION 36.0.0)
  set(BUILD_TOOLS ${CMAKE_CURRENT_SOURCE_DIR}/android/sdk/${CMAKE_HOST_SYSTEM_NAME}/build-tools/${BUILD_TOOLS_VERSION})

  set(CMAKE_SYSTEM_NAME "Android")
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
  set(LINK_LIBRARIES native_app_glue)

  add_library(main SHARED ${SRCS})
else()
  find_package(PkgConfig REQUIRED)
  pkg_check_modules(CURL REQUIRED libcurl)
  pkg_check_modules(ZLIB REQUIRED zlib)

  add_executable(main ${SRCS})
endif()

project(Main)

target_link_libraries(main PUBLIC ${CURL_LIBRARIES})
target_include_directories(main PUBLIC ${CURL_INCLUDE_DIRS})
target_compile_options(main PUBLIC ${CUR_CFLAGS_OTHER})

target_link_libraries(main PUBLIC ${ZLIB_LIBRARIES})
target_include_directories(main PUBLIC ${ZLIB_INCLUDE_DIRS})
target_compile_options(main PUBLIC ${ZLIB_CFLAGS_OTHER})

target_include_directories(main PUBLIC ./externals/lua/src/)

add_subdirectory(./externals/raylib/)
target_link_libraries(main PUBLIC raylib)

add_subdirectory(./externals/sol2)
target_link_libraries(main PUBLIC sol2)

add_subdirectory(./externals/json)
target_link_libraries(main PUBLIC nlohmann_json::nlohmann_json)


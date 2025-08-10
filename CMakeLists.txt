cmake_minimum_required(VERSION 3.12)

project(Main)
find_package(PkgConfig REQUIRED)

set(SRCS
  ./src/main.cpp
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(PLATFORM STREQUAL "Android")
  add_library(main SHARED ${SRCS})

  set(OPENSSL_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/externals/openssl/${CMAKE_ANDROID_ARCH_ABI} CACHE PATH "OpenSSL root")
  set(OPENSSL_INCLUDE_DIR ${OPENSSL_ROOT_DIR}/include CACHE PATH "OpenSSL include")
  set(OPENSSL_CRYPTO_LIBRARY ${OPENSSL_ROOT_DIR}/lib/libcrypto.a CACHE FILEPATH "OpenSSL crypto lib")
  set(OPENSSL_SSL_LIBRARY ${OPENSSL_ROOT_DIR}/lib/libssl.a CACHE FILEPATH "OpenSSL ssl lib")
  set(OPENSSL_LIBRARIES ${OPENSSL_SSL_LIBRARY} ${OPENSSL_CRYPTO_LIBRARY} CACHE STRING "OpenSSL libraries")
  set(OPENSSL_USE_STATIC_LIBS ON CACHE BOOL "Use static OpenSSL libs")

  find_package(OpenSSL REQUIRED)

  set(CURL_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/externals/curl/${CMAKE_ANDROID_ARCH_ABI}/include/)
  set(CURL_LIBRARIES ${CMAKE_CURRENT_SOURCE_DIR}/externals/curl/${CMAKE_ANDROID_ARCH_ABI}/lib/libcurl.a)
  set(CURL_LIBRARY ${CURL_LIBRARIES})
  set(CPR_USE_SYSTEM_CURL ON)
  set(ZLIB_LIBRARIES "-lz")

  add_library(native_app_glue STATIC
    ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c
  )
  target_include_directories(main PUBLIC
    ${ANDROID_NDK}/sources/android/native_app_glue
  )
  target_link_libraries(main PUBLIC
    "-Wl,--whole-archive" native_app_glue "-Wl,--no-whole-archive"
    android log
  )

  target_include_directories(main PUBLIC ${OPENSSL_INCLUDE_DIR})
  target_link_directories(main PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/externals/curl/${CMAKE_ANDROID_ARCH_ABI}/lib/)
  target_link_libraries(main PUBLIC ${OPENSSL_SSL_LIBRARY} ${OPENSSL_CRYPTO_LIBRARY})
else()
  add_executable(main ${SRCS})

  set(CPR_USE_SYSTEM_CURL ON)
  pkg_check_modules(CURL REQUIRED libcurl)
  pkg_check_modules(ZLIB REQUIRED zlib)
endif()

set_target_properties(main PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES)

add_library(lua STATIC ./externals/lua/src/lapi.c ./externals/lua/src/lcode.c ./externals/lua/src/lctype.c ./externals/lua/src/ldebug.c ./externals/lua/src/ldo.c ./externals/lua/src/ldump.c ./externals/lua/src/lfunc.c ./externals/lua/src/lgc.c ./externals/lua/src/llex.c ./externals/lua/src/lmem.c ./externals/lua/src/lobject.c ./externals/lua/src/lopcodes.c ./externals/lua/src/lparser.c ./externals/lua/src/lstate.c ./externals/lua/src/lstring.c ./externals/lua/src/ltable.c ./externals/lua/src/ltm.c ./externals/lua/src/lundump.c ./externals/lua/src/lvm.c ./externals/lua/src/lzio.c ./externals/lua/src/lauxlib.c ./externals/lua/src/lbaselib.c ./externals/lua/src/lcorolib.c ./externals/lua/src/ldblib.c ./externals/lua/src/liolib.c ./externals/lua/src/lmathlib.c ./externals/lua/src/loadlib.c ./externals/lua/src/loslib.c ./externals/lua/src/lstrlib.c ./externals/lua/src/ltablib.c ./externals/lua/src/lutf8lib.c ./externals/lua/src/linit.c)
target_compile_definitions(lua PUBLIC LUA_COMPAT_5_3)
target_link_libraries(main PUBLIC lua)

target_link_libraries(main PUBLIC ${CURL_LIBRARIES})
target_include_directories(main PUBLIC ${CURL_INCLUDE_DIR})
target_compile_options(main PUBLIC ${CURL_CFLAGS_OTHER})

target_link_libraries(main PUBLIC ${ZLIB_LIBRARIES})
target_include_directories(main PUBLIC ${ZLIB_INCLUDE_DIR})
target_compile_options(main PUBLIC ${ZLIB_CFLAGS_OTHER})

target_include_directories(main PUBLIC ./externals/lua/src/)

add_subdirectory(./externals/raylib/)
target_link_libraries(main PUBLIC raylib)

add_subdirectory(./externals/sol2)
target_link_libraries(main PUBLIC sol2)

add_subdirectory(./externals/json)
target_link_libraries(main PUBLIC nlohmann_json::nlohmann_json)

add_subdirectory(./externals/cpr)
target_link_libraries(main PUBLIC cpr::cpr)

file(READ "${CMAKE_SOURCE_DIR}/.env" DOTENV)

string(REGEX MATCH "SUPABASE_URL=([^\n\r]+)" SUPABASE_URL_LINE "${DOTENV}")
string(REGEX MATCH "SUPABASE_KEY=([^\n\r]+)" SUPABASE_KEY_LINE "${DOTENV}")

# Extract value after '='
string(REGEX REPLACE "SUPABASE_URL=(.+)" "\\1" SUPABASE_URL_RAW "${SUPABASE_URL_LINE}")
string(REGEX REPLACE "SUPABASE_KEY=(.+)" "\\1" SUPABASE_KEY_RAW "${SUPABASE_KEY_LINE}")

# Strip ALL single and double quotes from start and end
string(REGEX REPLACE "^['\"]+|['\"]+$" "" SUPABASE_URL_CLEAN "${SUPABASE_URL_RAW}")
string(REGEX REPLACE "^['\"]+|['\"]+$" "" SUPABASE_KEY_CLEAN "${SUPABASE_KEY_RAW}")

# Now add double quotes to pass as proper C++ string literal macros
set(SUPABASE_URL_DEF "\"${SUPABASE_URL_CLEAN}\"")
set(SUPABASE_KEY_DEF "\"${SUPABASE_KEY_CLEAN}\"")

target_compile_definitions(main PRIVATE
  SUPABASE_URL=${SUPABASE_URL_DEF}
  SUPABASE_KEY=${SUPABASE_KEY_DEF}
  PACKAGE_NAME="${PACKAGE_NAME}"
)

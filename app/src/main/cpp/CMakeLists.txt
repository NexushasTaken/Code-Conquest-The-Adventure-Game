# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Set the minimum required version of CMake
cmake_minimum_required(VERSION 3.12)

# Set standards for C and C++
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)

# Set the project name based on the name given on the gradle.properties
project("${APP_LIB_NAME}")

# Fetch all source files for your project (recursively), excluding 'deps' source files
set(SOURCES
  "${CMAKE_SOURCE_DIR}/main.cpp"
  "${CMAKE_SOURCE_DIR}/supabase/errorcodes.cpp"
  "${CMAKE_SOURCE_DIR}/nuklear/nuklear.c"
)

if(PLATFORM STREQUAL "Android")
  add_library(${APP_LIB_NAME} SHARED ${SOURCES})

  set(OPENSSL_ROOT_DIR ${CMAKE_SOURCE_DIR}/deps/openssl/${CMAKE_ANDROID_ARCH_ABI} CACHE PATH "OpenSSL root")
  set(OPENSSL_INCLUDE_DIR ${OPENSSL_ROOT_DIR}/include CACHE PATH "OpenSSL include")
  set(OPENSSL_CRYPTO_LIBRARY ${OPENSSL_ROOT_DIR}/lib/libcrypto.a CACHE FILEPATH "OpenSSL crypto lib")
  set(OPENSSL_SSL_LIBRARY ${OPENSSL_ROOT_DIR}/lib/libssl.a CACHE FILEPATH "OpenSSL ssl lib")
  set(OPENSSL_LIBRARIES ${OPENSSL_SSL_LIBRARY} ${OPENSSL_CRYPTO_LIBRARY} CACHE STRING "OpenSSL libraries")
  set(OPENSSL_USE_STATIC_LIBS ON CACHE BOOL "Use static OpenSSL libs")

  find_package(OpenSSL REQUIRED)

  set(CURL_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/deps/curl/${CMAKE_ANDROID_ARCH_ABI}/include/)
  set(CURL_LIBRARIES ${CMAKE_SOURCE_DIR}/deps/curl/${CMAKE_ANDROID_ARCH_ABI}/lib/libcurl.a)
  set(CURL_LIBRARY ${CURL_LIBRARIES})
  set(CPR_USE_SYSTEM_CURL ON)
  set(ZLIB_LIBRARIES "-lz")

  add_library(native_app_glue STATIC
    ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c
  )
  target_include_directories(${APP_LIB_NAME} PUBLIC
    ${ANDROID_NDK}/sources/android/native_app_glue
  )
  target_link_libraries(${APP_LIB_NAME} PUBLIC
    "-Wl,--whole-archive" native_app_glue "-Wl,--no-whole-archive"
    android log
  )

  target_include_directories(${APP_LIB_NAME} PUBLIC ${OPENSSL_INCLUDE_DIR})
  target_link_directories(${APP_LIB_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/deps/curl/${CMAKE_ANDROID_ARCH_ABI}/lib/)
  target_link_libraries(${APP_LIB_NAME} PUBLIC ${OPENSSL_SSL_LIBRARY} ${OPENSSL_CRYPTO_LIBRARY})
else()
  add_executable(${APP_LIB_NAME} ${SOURCES})

  set(CPR_USE_SYSTEM_CURL ON)
  pkg_check_modules(CURL REQUIRED libcurl)
  pkg_check_modules(ZLIB REQUIRED zlib)
endif()

# Apply flags depending on the build type
if(CMAKE_BUILD_TYPE MATCHES "Debug")
    target_compile_definitions(${APP_LIB_NAME} PRIVATE _DEBUG DEBUG)
endif()

add_library(lua STATIC ${CMAKE_SOURCE_DIR}/deps/lua/src/lapi.c ${CMAKE_SOURCE_DIR}/deps/lua/src/lcode.c ${CMAKE_SOURCE_DIR}/deps/lua/src/lctype.c ${CMAKE_SOURCE_DIR}/deps/lua/src/ldebug.c ${CMAKE_SOURCE_DIR}/deps/lua/src/ldo.c ${CMAKE_SOURCE_DIR}/deps/lua/src/ldump.c ${CMAKE_SOURCE_DIR}/deps/lua/src/lfunc.c ${CMAKE_SOURCE_DIR}/deps/lua/src/lgc.c ${CMAKE_SOURCE_DIR}/deps/lua/src/llex.c ${CMAKE_SOURCE_DIR}/deps/lua/src/lmem.c ${CMAKE_SOURCE_DIR}/deps/lua/src/lobject.c ${CMAKE_SOURCE_DIR}/deps/lua/src/lopcodes.c ${CMAKE_SOURCE_DIR}/deps/lua/src/lparser.c ${CMAKE_SOURCE_DIR}/deps/lua/src/lstate.c ${CMAKE_SOURCE_DIR}/deps/lua/src/lstring.c ${CMAKE_SOURCE_DIR}/deps/lua/src/ltable.c ${CMAKE_SOURCE_DIR}/deps/lua/src/ltm.c ${CMAKE_SOURCE_DIR}/deps/lua/src/lundump.c ${CMAKE_SOURCE_DIR}/deps/lua/src/lvm.c ${CMAKE_SOURCE_DIR}/deps/lua/src/lzio.c ${CMAKE_SOURCE_DIR}/deps/lua/src/lauxlib.c ${CMAKE_SOURCE_DIR}/deps/lua/src/lbaselib.c ${CMAKE_SOURCE_DIR}/deps/lua/src/lcorolib.c ${CMAKE_SOURCE_DIR}/deps/lua/src/ldblib.c ${CMAKE_SOURCE_DIR}/deps/lua/src/liolib.c ${CMAKE_SOURCE_DIR}/deps/lua/src/lmathlib.c ${CMAKE_SOURCE_DIR}/deps/lua/src/loadlib.c ${CMAKE_SOURCE_DIR}/deps/lua/src/loslib.c ${CMAKE_SOURCE_DIR}/deps/lua/src/lstrlib.c ${CMAKE_SOURCE_DIR}/deps/lua/src/ltablib.c ${CMAKE_SOURCE_DIR}/deps/lua/src/lutf8lib.c ${CMAKE_SOURCE_DIR}/deps/lua/src/linit.c)
target_compile_definitions(lua PUBLIC LUA_COMPAT_5_3)
target_link_libraries(${APP_LIB_NAME} PUBLIC lua)

target_link_libraries(${APP_LIB_NAME} PUBLIC ${CURL_LIBRARIES})
target_include_directories(${APP_LIB_NAME} PUBLIC ${CURL_INCLUDE_DIR})
target_compile_options(${APP_LIB_NAME} PUBLIC ${CURL_CFLAGS_OTHER})

target_link_libraries(${APP_LIB_NAME} PUBLIC ${ZLIB_LIBRARIES})
target_include_directories(${APP_LIB_NAME} PUBLIC ${ZLIB_INCLUDE_DIR})
target_compile_options(${APP_LIB_NAME} PUBLIC ${ZLIB_CFLAGS_OTHER})

target_include_directories(${APP_LIB_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/deps/lua/src/)

add_subdirectory(${CMAKE_SOURCE_DIR}/deps/raylib/)
target_link_libraries(${APP_LIB_NAME} PUBLIC raylib)

add_subdirectory(${CMAKE_SOURCE_DIR}/deps/sol2)
target_link_libraries(${APP_LIB_NAME} PUBLIC sol2)

add_subdirectory(${CMAKE_SOURCE_DIR}/deps/json)
target_link_libraries(${APP_LIB_NAME} PUBLIC nlohmann_json::nlohmann_json)

set(JWT_BUILD_EXAMPLES OFF)
add_subdirectory(${CMAKE_SOURCE_DIR}/deps/jwt-cpp/)
target_link_libraries(${APP_LIB_NAME} PUBLIC jwt-cpp)

add_subdirectory(${CMAKE_SOURCE_DIR}/deps/cpr)
target_link_libraries(${APP_LIB_NAME} PUBLIC cpr::cpr)

add_subdirectory(${CMAKE_SOURCE_DIR}/deps/raymob)
target_include_directories(${APP_LIB_NAME} PUBLIC "${CMAKE_SOURCE_DIR}/deps/raymob")
target_link_libraries(${APP_LIB_NAME} PUBLIC raymoblib)

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

target_compile_definitions(${APP_LIB_NAME} PRIVATE
  SUPABASE_URL=${SUPABASE_URL_DEF}
  SUPABASE_KEY=${SUPABASE_KEY_DEF}
  PACKAGE_NAME="${PACKAGE_NAME}"
)

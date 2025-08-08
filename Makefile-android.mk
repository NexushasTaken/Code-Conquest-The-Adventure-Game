# ==============================================================================
# Paths
# ==============================================================================
KERNEL_NAME := $(shell uname)
BUILD_DIR := $(CURDIR)/android/build
SDK := $(CURDIR)/android/sdk/$(KERNEL_NAME)
NDK := $(CURDIR)/android/ndk/$(KERNEL_NAME)/android-ndk-r25b
JAVA_HOME := /usr/lib/jvm/java-21-openjdk
BUILD_TOOLS := $(SDK)/build-tools/36.0.0
TOOLCHAIN := $(NDK)/toolchains/llvm/prebuilt/linux-x86_64
NATIVE_APP_GLUE := $(NDK)/sources/android/native_app_glue
OPENSSL_VERSION := 3.5.1
OPENSSL_ROOT := ./externals/openssl/openssl_$(OPENSSL_VERSION)_$(ABI)/

# ==============================================================================
# Tools
# ==============================================================================
AR := $(TOOLCHAIN)/bin/llvm-ar

# ==============================================================================
# ABI-specific settings
# ==============================================================================
ifneq (,$(filter $(ABI),armeabi-v7a arm64-v8a x86 x86_64))
  ifeq ($(ABI),armeabi-v7a)
    CCTYPE := armv7a-linux-androideabi
    ARCH := arm
    ARCH_ALT := $(ARCH)
    LIBPATH := arm-linux-androideabi
    ABI_FLAGS := -march=armv7-a -mfloat-abi=softfp -mfpu=vfpv3-d16
  endif
  ifeq ($(ABI),arm64-v8a)
    CCTYPE := aarch64-linux-android
    ARCH := aarch64
    ARCH_ALT := arm64
    LIBPATH := aarch64-linux-android
    ABI_FLAGS := -target aarch64 -mfix-cortex-a53-835769
  endif
  ifeq ($(ABI),x86)
    CCTYPE := i686-linux-android
    ARCH := i386
    ARCH_ALT := x86
    LIBPATH := i686-linux-android
    ABI_FLAGS :=
  endif
  ifeq ($(ABI),x86_64)
    CCTYPE := x86_64-linux-android
    ARCH := x86_64
    ARCH_ALT := $(ARCH)
    LIBPATH := x86_64-linux-android
    ABI_FLAGS :=
  endif
else
  $(error Unsupported ABI: $(ABI). Supported: armeabi-v7a, arm64-v8a, x86, x86_64)
endif

CC := $(TOOLCHAIN)/bin/$(CCTYPE)$(API_VERSION)-clang
CXX := $(TOOLCHAIN)/bin/$(CCTYPE)$(API_VERSION)-clang++

# ==============================================================================
# Output Files and Directories
# ==============================================================================
LIB_DIR := lib/$(TARGET)/$(ABI)
LIBRAYLIB := $(LIB_DIR)/libraylib.a
LIBNATIVE_APP_GLUE := $(LIB_DIR)/libnative_app_glue.a
LIBMAIN := lib/$(ABI)/libmain.so
R_JAVA := $(BUILD_DIR)/src/$(DOMAIN)/$(DEV_NAME)/$(NAME)/R.java
NATIVE_LOADER_SRC := $(BUILD_DIR)/src/$(DOMAIN)/$(DEV_NAME)/$(NAME)/NativeLoader.java
NATIVE_LOADER_CLASS := $(BUILD_DIR)/classes/$(DOMAIN)/$(DEV_NAME)/$(NAME)/NativeLoader.class
CLASSES_DEX := $(BUILD_DIR)/dex/classes.dex
APK := $(NAME).apk
ALIGNED_APK := $(NAME).aligned.apk
SIGNED_APK := $(DOMAIN).$(DEV_NAME).$(NAME).apk
ASSET_DIR := ./assets

# Resource files (explicitly listed to avoid wildcard issues)
RES_FILES := $(wildcard $(BUILD_DIR)/res/*/*) $(BUILD_DIR)/AndroidManifest.xml
ASSET_FILES := $(shell find $(ASSET_DIR) -type f)

SRC := src/main.cpp
SRC += externals/cpr/cpr/accept_encoding.cpp externals/cpr/cpr/async.cpp externals/cpr/cpr/auth.cpp externals/cpr/cpr/callback.cpp externals/cpr/cpr/cert_info.cpp externals/cpr/cpr/cookies.cpp externals/cpr/cpr/cprtypes.cpp externals/cpr/cpr/curl_container.cpp externals/cpr/cpr/curlholder.cpp externals/cpr/cpr/error.cpp externals/cpr/cpr/file.cpp externals/cpr/cpr/multipart.cpp externals/cpr/cpr/parameters.cpp externals/cpr/cpr/payload.cpp externals/cpr/cpr/proxies.cpp externals/cpr/cpr/proxyauth.cpp externals/cpr/cpr/session.cpp externals/cpr/cpr/threadpool.cpp externals/cpr/cpr/timeout.cpp externals/cpr/cpr/unix_socket.cpp externals/cpr/cpr/util.cpp externals/cpr/cpr/response.cpp externals/cpr/cpr/redirect.cpp externals/cpr/cpr/interceptor.cpp externals/cpr/cpr/ssl_ctx.cpp externals/cpr/cpr/curlmultiholder.cpp externals/cpr/cpr/multiperform.cpp
OBJ := $(SRC:%=$(LIB_DIR)/%.o)

LUA_SRC := ./externals/lua/src/lapi.c ./externals/lua/src/lcode.c ./externals/lua/src/lctype.c ./externals/lua/src/ldebug.c ./externals/lua/src/ldo.c ./externals/lua/src/ldump.c ./externals/lua/src/lfunc.c ./externals/lua/src/lgc.c ./externals/lua/src/llex.c ./externals/lua/src/lmem.c ./externals/lua/src/lobject.c ./externals/lua/src/lopcodes.c ./externals/lua/src/lparser.c ./externals/lua/src/lstate.c ./externals/lua/src/lstring.c ./externals/lua/src/ltable.c ./externals/lua/src/ltm.c ./externals/lua/src/lundump.c ./externals/lua/src/lvm.c ./externals/lua/src/lzio.c ./externals/lua/src/lauxlib.c ./externals/lua/src/lbaselib.c ./externals/lua/src/lcorolib.c ./externals/lua/src/ldblib.c ./externals/lua/src/liolib.c ./externals/lua/src/lmathlib.c ./externals/lua/src/loadlib.c ./externals/lua/src/loslib.c ./externals/lua/src/lstrlib.c ./externals/lua/src/ltablib.c ./externals/lua/src/lutf8lib.c ./externals/lua/src/linit.c
LUA_OBJ := $(LUA_SRC:%=$(LIB_DIR)/%.o)

# ==============================================================================
# Compiler Flags
# ==============================================================================
# === Android-specific flags ===
ANDROID_FLAGS := -DANDROID -DPLATFORM_ANDROID -D__ANDROID_API__=$(API_VERSION) -u ANativeActivity_onCreate
ANDROID_FLAGS += -no-canonical-prefixes -fstack-protector-strong -funwind-tables -ffunction-sections -fPIC

# === Compiler warnings and compatibility ===
CFLAGS += -Wall -Wa,--noexecstack -Wformat -Werror=format-security -Wno-c++11-narrowing -DLUA_COMPAT_5_3

# === C++ specific flags ===
CXXFLAGS += -std=c++17

# === Linker flags (Android/libs/system) ===
LD_FLAGS := -Wl,--build-id -Wl,-z,noexecstack -Wl,-z,relro -Wl,-z,now -Wl,--warn-shared-textrel -Wl,--fatal-warnings \
	-L$(TOOLCHAIN)/sysroot/usr/lib/$(LIBPATH)/$(API_VERSION) -L$(TOOLCHAIN)/lib/clang/17/lib/linux/$(ARCH) \
	-lraylib -lnative_app_glue -llog -landroid -lEGL -lGLESv2 -lOpenSLES -lz -lc -lm -ldl -latomic -lcurl -lssl -lcrypto
LD_FLAGS += -L. -L$(BUILD_DIR)/obj -L$(LIB_DIR)
LD_FLAGS += -Lexternals/curl/${ABI}/lib
LD_FLAGS += -Lexternals/openssl/${ABI}/lib

# === Include directories ===
INCLUDES := -I. -Isrc -Iinclude -I../include -I$(NATIVE_APP_GLUE) -I$(TOOLCHAIN)/sysroot/usr/include
INCLUDES += -Iexternals/curl/${ABI}/include
INCLUDES += -Lexternals/openssl/${ABI}/include
INCLUDES += -Iexternals/imgui
INCLUDES += -Iexternals/rlImGui
INCLUDES += -Iexternals/raylib/src
INCLUDES += -Iexternals/sol2/include
INCLUDES += -Iexternals/lua/src
INCLUDES += -Iexternals/json/include
INCLUDES += -Iexternals/cpr/include

FLAGS := $(INCLUDES) $(ANDROID_FLAGS)

# ==============================================================================
# Targets
# ==============================================================================

# Default target: Build the signed APK
.PHONY: all
all: $(SIGNED_APK)

# Build Raylib static library
$(LIBRAYLIB):
	$(MAKE) -C externals/raylib/src clean
	$(MAKE) -C externals/raylib/src PLATFORM=PLATFORM_ANDROID \
		ANDROID_NDK=$(NDK) ANDROID_ARCH=$(ARCH_ALT) \
		ANDROID_API_VERSION=$(API_VERSION) RAYLIB_BUILD_MODE=RELEASE -j
	@mkdir -p $(LIB_DIR)
	mv externals/raylib/src/libraylib.a $@
	$(MAKE) -C externals/raylib/src clean

# Compile Android native app glue
$(LIBNATIVE_APP_GLUE): $(NATIVE_APP_GLUE)/android_native_app_glue.c
	@mkdir -p $(LIB_DIR)
	$(CC) -c $< -o $(NATIVE_APP_GLUE)/native_app_glue.o \
		-I$(TOOLCHAIN)/sysroot/usr/include/$(CCTYPE) $(FLAGS) $(CFLAGS) $(ANDROID_FLAGS)
	$(AR) rcs $@ $(NATIVE_APP_GLUE)/native_app_glue.o

$(LIB_DIR)/%.c.o: %.c
	@mkdir -p $(@D)
	$(CC) -c $< -o $@ $(FLAGS) $(CFLAGS)

$(LIB_DIR)/%.cpp.o: %.cpp
	@mkdir -p $(@D)
	$(CXX) -c $< -o $@ $(FLAGS) $(CXXFLAGS)

# Compile main shared library (libmain.so)
$(LIBMAIN): $(OBJ) $(LUA_OBJ) $(LIBRAYLIB) $(LIBNATIVE_APP_GLUE)
	@mkdir -p $(@D)
	$(CXX) $(OBJ) $(LUA_OBJ) -o $@ -shared $(FLAGS) $(CFLAGS) $(CXXFLAGS) $(LD_FLAGS) $(ANDROID_FLAGS)

# Generate R.java from resources
$(R_JAVA): $(RES_FILES)
	@mkdir -p $(@D)
	$(BUILD_TOOLS)/aapt package -f -m \
		-S $(BUILD_DIR)/res -J $(BUILD_DIR)/src \
		-M $(BUILD_DIR)/AndroidManifest.xml \
		-I $(SDK)/platforms/android-$(API_VERSION)/android.jar

# Compile NativeLoader.java and R.java
$(NATIVE_LOADER_CLASS): $(R_JAVA) $(NATIVE_LOADER_SRC)
	@mkdir -p $(BUILD_DIR)/classes
	$(JAVA_HOME)/bin/javac -verbose -source 1.8 -target 1.8 \
		-classpath $(SDK)/platforms/android-$(API_VERSION)/android.jar \
		-sourcepath $(BUILD_DIR)/src $(R_JAVA) $(NATIVE_LOADER_SRC) \
		-d $(BUILD_DIR)/classes

# Generate classes.dex from Java classes
$(CLASSES_DEX): $(NATIVE_LOADER_CLASS)
	@mkdir -p $(BUILD_DIR)/dex
	$(BUILD_TOOLS)/d8 --classpath $(SDK)/platforms/android-$(API_VERSION)/android.jar \
		--output $(BUILD_DIR)/dex $(BUILD_DIR)/classes/$(DOMAIN)/$(DEV_NAME)/$(NAME)/*.class

# Package APK with resources, assets, and DEX
$(APK): $(CLASSES_DEX) $(LIBMAIN) $(ASSET_FILES)
	$(BUILD_TOOLS)/aapt package -f \
		-M $(BUILD_DIR)/AndroidManifest.xml -S $(BUILD_DIR)/res -A assets \
		-I $(SDK)/platforms/android-$(API_VERSION)/android.jar -F $@ $(BUILD_DIR)/dex
	cp -v $(TOOLCHAIN)/sysroot/usr/lib/$(LIBPATH)/libc++_shared.so lib/$(ABI)/
	$(BUILD_TOOLS)/aapt add $@ $(LIBMAIN) lib/$(ABI)/libc++_shared.so

# Zipalign the APK
$(ALIGNED_APK): $(APK)
	$(BUILD_TOOLS)/zipalign -f 4 $< $@

# Sign the APK
$(SIGNED_APK): $(ALIGNED_APK)
	$(BUILD_TOOLS)/apksigner sign \
		--ks android/$(DEV_NAME).keystore \
		--ks-pass pass:$(DEV_NAME) \
		--out $@ $<

externals/openssl/OpenSSL_$(OPENSSL_VERSION)_$(ABI).tar.gz:
	@mkdir -p $(@D)
	wget https://github.com/217heidai/openssl_for_android/releases/download/$(OPENSSL_VERSION)/OpenSSL_$(OPENSSL_VERSION)_$(ABI).tar.gz -O $@
	tar xvf $@ --directory $(@D)

# Install APK to device/emulator
.PHONY: install
install: stop $(SIGNED_APK)
	$(SDK)/platform-tools/adb install -r $(SIGNED_APK)

.PHONY: run
run: install
	sleep 1
	$(SDK)/platform-tools/adb shell am start -n $(PACKAGE_NAME)/$(PACKAGE_NAME).NativeLoader

.PHONY: stop
stop:
	$(SDK)/platform-tools/adb shell am force-stop $(PACKAGE_NAME)

# Clean build artifacts
.PHONY: clean clean-apk clean-main
clean: clean-apk
	rm -rf $(LIB_DIR) lib
	$(MAKE) -C externals/raylib/src clean

clean-apk:
	rm -rf $(APK) $(ALIGNED_APK) $(SIGNED_APK) $(SIGNED_APK).idsig

clean-main:
	rm -rf $(LIBMAIN)

# ==============================================================================
# Usage:
#   Build: make ABI=armeabi-v7a
#   Build and install: make ABI=arm64-v8a install
#   Clean: make clean
# ==============================================================================

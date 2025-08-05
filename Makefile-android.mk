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

OBJ := $(SRC:%=$(LIB_DIR)/%.o)

# ==============================================================================
# Compiler Flags
# ==============================================================================
FLAGS := -ffunction-sections -funwind-tables -fstack-protector-strong -fPIC -Wall \
         -Wa,--noexecstack -Wformat -Werror=format-security -no-canonical-prefixes \
         -DANDROID -DPLATFORM_ANDROID -D__ANDROID_API__=$(API_VERSION) -std=c++20 -Wno-c++11-narrowing
LD_FLAGS := -Wl,--build-id -Wl,-z,noexecstack -Wl,-z,relro -Wl,-z,now \
		-Wl,--warn-shared-textrel -Wl,--fatal-warnings \
		-L$(TOOLCHAIN)/sysroot/usr/lib/$(LIBPATH)/$(API_VERSION) \
		-L$(TOOLCHAIN)/lib/clang/17/lib/linux/$(ARCH) \
		-L. -L$(BUILD_DIR)/obj -L$(LIB_DIR) -L./externals/curl/${ABI} \
		-lraylib -lnative_app_glue -llog -landroid -lEGL -lGLESv2 -lOpenSLES -lz -lc -lm -ldl -lcurl -latomic

INCLUDES += -I. -Isrc -Iinclude -I../include -I$(NATIVE_APP_GLUE) -I$(TOOLCHAIN)/sysroot/usr/include
INCLUDES += -Iexternals/curl/include
INCLUDES += -Iexternals/imgui -Iexternals/rlImGui
INCLUDES += -Iexternals/raylib/src

SRC_FLAGS := -u ANativeActivity_onCreate $(INCLUDES) $(FLAGS) $(ABI_FLAGS)

# ==============================================================================
# Targets
# ==============================================================================

# Default target: Build the signed APK
.PHONY: all
all: $(SIGNED_APK)

# Copy icons to resolution-specific directories
$(BUILD_DIR)/res/drawable-ldpi/icon.png: assets/icon_ldpi.png
	@mkdir -p $(@D)
	cp $< $@

$(BUILD_DIR)/res/drawable-mdpi/icon.png: assets/icon_mdpi.png
	@mkdir -p $(@D)
	cp $< $@

$(BUILD_DIR)/res/drawable-hdpi/icon.png: assets/icon_hdpi.png
	@mkdir -p $(@D)
	cp $< $@

$(BUILD_DIR)/res/drawable-xhdpi/icon.png: assets/icon_xhdpi.png
	@mkdir -p $(@D)
	cp $< $@

# Copy assets to build directory
$(BUILD_DIR)/assets: $(ASSET_FILES)
	@mkdir -p $@
	cp -r assets/* $@

# Aggregate asset copying
.PHONY: copy-assets
copy-assets: $(BUILD_DIR)/res/drawable-ldpi/icon.png \
             $(BUILD_DIR)/res/drawable-mdpi/icon.png \
             $(BUILD_DIR)/res/drawable-hdpi/icon.png \
             $(BUILD_DIR)/res/drawable-xhdpi/icon.png \
             $(BUILD_DIR)/assets

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
	$(CC) -c $< -o $(NATIVE_APP_GLUE)/native_app_glue.o $(INCLUDES) \
		-I$(TOOLCHAIN)/sysroot/usr/include/$(CCTYPE) $(FLAGS) $(ABI_FLAGS)
	$(AR) rcs $@ $(NATIVE_APP_GLUE)/native_app_glue.o

$(LIB_DIR)/%.cpp.o: %.cpp
	@mkdir -p $(@D)
	$(CXX) -c $< -o $@ $(SRC_FLAGS) $(FLAGS)

# Compile main shared library (libmain.so)
$(LIBMAIN): $(OBJ) $(LIBRAYLIB) $(LIBNATIVE_APP_GLUE)
	@mkdir -p $(@D)
	$(CXX) $(OBJ) -o $@ -shared $(SRC_FLAGS) $(LD_FLAGS)

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

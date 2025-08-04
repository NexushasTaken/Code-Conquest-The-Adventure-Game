# Configuration
NAME := game
DEV_NAME := raylib
API_VERSION := 31
ABI ?= armeabi-v7a
SRC := src/*.cpp
TARGET := Android

# Paths
BUILD := $(CURDIR)/android/build
SDK := $(CURDIR)/android/sdk/$(shell uname)
NDK := $(CURDIR)/android/ndk/$(shell uname)/android-ndk-r25b
JAVA := /usr/lib/jvm/java-21-openjdk
BUILD_TOOLS := $(SDK)/build-tools/36.0.0
TOOLCHAIN := $(NDK)/toolchains/llvm/prebuilt/linux-x86_64
NATIVE_APP_GLUE := $(NDK)/sources/android/native_app_glue

# Compiler and tools
AR := $(TOOLCHAIN)/bin/llvm-ar

# ABI-specific settings
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

CC := $(TOOLCHAIN)/bin/$(CCTYPE)$(API_VERSION)-clang
CXX := $(TOOLCHAIN)/bin/$(CCTYPE)$(API_VERSION)-clang++

# Flags
FLAGS := -ffunction-sections -funwind-tables -fstack-protector-strong -fPIC -Wall \
  -Wa,--noexecstack -Wformat -Werror=format-security -no-canonical-prefixes \
  -DANDROID -DPLATFORM_ANDROID -D__ANDROID_API__=$(API_VERSION)
INCLUDES := -I. -Iinclude -I../include -I$(NATIVE_APP_GLUE) -I$(TOOLCHAIN)/sysroot/usr/include

# Output directories and files
LIB_DIR := lib/$(TARGET)/$(ABI)
LIBRAYLIB := $(LIB_DIR)/libraylib.a
LIBNATIVE_APP_GLUE := $(LIB_DIR)/libnative_app_glue.a
LIBMAIN := $(BUILD)/lib/$(ABI)/libmain.so
R_JAVA := $(BUILD)/src/com/$(DEV_NAME)/$(NAME)/R.java
NATIVE_LOADER_CLASS := $(BUILD)/classes/com/$(DEV_NAME)/$(NAME)/NativeLoader.class
CLASSES_DEX := $(BUILD)/dex/classes.dex
APK := $(NAME).apk
ALIGNED_APK := $(NAME).aligned.apk
SIGNED_APK := $(NAME).signed.apk
hello:
	echo $(LIBMAIN)

# Default target
.PHONY: all
all: $(SIGNED_APK)

# Copy assets
$(BUILD)/res/drawable-ldpi/icon.png: assets/icon_ldpi.png
	@mkdir -p $(BUILD)/res/drawable-ldpi
	cp $< $@

$(BUILD)/res/drawable-mdpi/icon.png: assets/icon_mdpi.png
	@mkdir -p $(BUILD)/res/drawable-mdpi
	cp $< $@

$(BUILD)/res/drawable-hdpi/icon.png: assets/icon_hdpi.png
	@mkdir -p $(BUILD)/res/drawable-hdpi
	cp $< $@

$(BUILD)/res/drawable-xhdpi/icon.png: assets/icon_xhdpi.png
	@mkdir -p $(BUILD)/res/drawable-xhdpi
	cp $< $@

$(BUILD)/assets:
	@mkdir -p $(BUILD)/assets
	cp -r assets/* $(BUILD)/assets

.PHONY: copy-assets
copy-assets: $(BUILD)/res/drawable-ldpi/icon.png $(BUILD)/res/drawable-mdpi/icon.png \
             $(BUILD)/res/drawable-hdpi/icon.png $(BUILD)/res/drawable-xhdpi/icon.png \
             $(BUILD)/assets

# Compile Raylib
$(LIBRAYLIB):
	$(MAKE) -C externals/raylib/src clean
	$(MAKE) -C externals/raylib/src PLATFORM=PLATFORM_ANDROID ANDROID_NDK=$(NDK) \
	  ANDROID_ARCH=$(ARCH_ALT) ANDROID_API_VERSION=$(API_VERSION) RAYLIB_BUILD_MODE=RELEASE -j
	@mkdir -p $(LIB_DIR)
	mv externals/raylib/src/libraylib.a $(LIBRAYLIB)
	$(MAKE) -C externals/raylib/src clean

# Compile native app glue
$(LIBNATIVE_APP_GLUE): $(NATIVE_APP_GLUE)/android_native_app_glue.c
	@mkdir -p $(LIB_DIR)
	$(CC) -c $< -o $(NATIVE_APP_GLUE)/native_app_glue.o $(INCLUDES) \
	  -I$(TOOLCHAIN)/sysroot/usr/include/$(CCTYPE) $(FLAGS) $(ABI_FLAGS)
	$(AR) rcs $@ $(NATIVE_APP_GLUE)/native_app_glue.o

# Compile project (libmain.so)
$(LIBMAIN): $(SRC) $(LIBRAYLIB) $(LIBNATIVE_APP_GLUE)
	@mkdir -p $(BUILD)/lib/$(ABI)
	$(CXX) $(SRC) -o $@ -shared \
	  -Wl,--exclude-libs,libatomic.a \
	  -Wl,--build-id \
	  -Wl,-z,noexecstack \
	  -Wl,-z,relro \
	  -Wl,-z,now \
	  -Wl,--warn-shared-textrel \
	  -Wl,--fatal-warnings \
	  -u ANativeActivity_onCreate \
	  -L$(TOOLCHAIN)/sysroot/usr/lib/$(LIBPATH)/$(API_VERSION) \
	  -L$(TOOLCHAIN)/lib/clang/17/lib/linux/$(ARCH) \
	  -L. -L$(BUILD)/obj -L$(LIB_DIR) \
	  -lraylib -lnative_app_glue -llog -landroid -lEGL -lGLESv2 -lOpenSLES -lc -lm -ldl \
	  $(INCLUDES) $(FLAGS) $(ABI_FLAGS)

# Generate R.java
$(R_JAVA): $(BUILD)/AndroidManifest.xml copy-assets
	@mkdir -p $(dir $@)
	$(BUILD_TOOLS)/aapt package -f -m \
	  -S $(BUILD)/res -J $(BUILD)/src -M $(BUILD)/AndroidManifest.xml \
	  -I $(SDK)/platforms/android-$(API_VERSION)/android.jar

# Compile NativeLoader.java
$(NATIVE_LOADER_CLASS): $(R_JAVA) $(BUILD)/src/com/$(DEV_NAME)/$(NAME)/NativeLoader.java
	@mkdir -p $(BUILD)/classes
	$(JAVA)/bin/javac -verbose -source 1.8 -target 1.8 \
	  -classpath $(SDK)/platforms/android-$(API_VERSION)/android.jar \
	  -sourcepath $(BUILD)/src $(R_JAVA) \
	  -d $(BUILD)/classes \
	  $(BUILD)/src/com/$(DEV_NAME)/$(NAME)/NativeLoader.java

# Generate classes.dex
$(CLASSES_DEX): $(NATIVE_LOADER_CLASS)
	@mkdir -p $(BUILD)/dex
	$(BUILD_TOOLS)/d8 --classpath $(SDK)/platforms/android-$(API_VERSION)/android.jar \
	  --output $(BUILD)/dex $(BUILD)/classes/com/$(DEV_NAME)/$(NAME)/*.class

# Package APK
$(APK): $(CLASSES_DEX) $(LIBMAIN)
	$(BUILD_TOOLS)/aapt package -f \
	  -M $(BUILD)/AndroidManifest.xml -S $(BUILD)/res -A $(BUILD)/assets \
	  -I $(SDK)/platforms/android-$(API_VERSION)/android.jar -F $@ $(BUILD)/dex
	$(BUILD_TOOLS)/aapt add $@ $(LIBMAIN)

# Zipalign APK
$(ALIGNED_APK): $(APK)
	$(BUILD_TOOLS)/zipalign -f 4 $< $@

# Sign APK
$(SIGNED_APK): $(ALIGNED_APK)
	$(BUILD_TOOLS)/apksigner sign \
	  --ks android/$(DEV_NAME).keystore \
	  --ks-pass pass:$(DEV_NAME) \
	  --out $@ $<
	mv $@ $(APK)

# Install APK if -r is specified
.PHONY: install
install: $(APK)
	$(SDK)/platform-tools/adb install -r $<

# Clean
.PHONY: clean
clean:
	rm -rf $(BUILD) $(LIB_DIR) $(APK) $(ALIGNED_APK)
	$(MAKE) -C externals/raylib/src clean

# Run with `make ABI=armeabi-v7a` or `make ABI=arm64-v8a install` to build and install

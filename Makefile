# ==============================================================================
# Configuration
# ==============================================================================
DOMAIN := com
DEV_NAME := raylib
NAME := game
PACKAGE_NAME := $(DOMAIN).$(DEV_NAME).$(NAME)
API_VERSION := 31
ABI ?= armeabi-v7a
TARGET := Linux

SRC := src/main.cpp
SRC += externals/rlImGui/rlImGui.cpp
SRC += externals/imgui/imgui.cpp
SRC += externals/imgui/imgui_draw.cpp
SRC += externals/imgui/imgui_tables.cpp
SRC += externals/imgui/imgui_widgets.cpp
SRC += externals/imgui/imgui_demo.cpp

ifeq ($(TARGET),Android)
	include ./Makefile-android.mk
endif
ifeq ($(TARGET),Linux)
	include ./Makefile-linux.mk
endif

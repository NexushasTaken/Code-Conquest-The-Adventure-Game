# ==============================================================================
# Configuration
# ==============================================================================
DOMAIN := com
DEV_NAME := raylib
NAME := game
PACKAGE_NAME := $(DOMAIN).$(DEV_NAME).$(NAME)
API_VERSION := 31
ABI ?= armeabi-v7a
SRC := src/main.cpp
TARGET := Linux

ifeq ($(TARGET),Android)
	include ./Makefile-android.mk
endif
ifeq ($(TARGET),Linux)
	include ./Makefile-linux.mk
endif

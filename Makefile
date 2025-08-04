# ==============================================================================
# Configuration
# ==============================================================================
DOMAIN := com
NAME := game
DEV_NAME := raylib
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

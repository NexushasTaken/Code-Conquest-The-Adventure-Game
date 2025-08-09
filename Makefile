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
include .env
BUILD_TYPE := Debug

SRC := src/main.cpp
CFLAGS :=
CXXFLAGS := -DSUPABASE_URL=$(SUPABASE_URL) -DSUPABASE_KEY=$(SUPABASE_KEY) -DPACKAGE_NAME='"$(PACKAGE_NAME)"'

ifeq ($(BUILD_TYPE),Debug)
	CFLAGS += -ggdb
	CXXFLAGS += -ggdb
endif
ifeq ($(BUILD_TYPE),Release)
	CFLAGS += -O2
	CXXFLAGS += -O2
endif

ifeq ($(TARGET),Android)
	include ./Makefile-android.mk
endif
ifeq ($(TARGET),Linux)
	include ./Makefile-linux.mk
endif

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

SRC := src/main.cpp
CFLAGS := -DSUPABASE_URL=$(SUPABASE_URL) -DSUPABASE_KEY=$(SUPABASE_KEY)
CXXFLAGS := $(CFLAGS)

ifeq ($(TARGET),Android)
	include ./Makefile-android.mk
endif
ifeq ($(TARGET),Linux)
	include ./Makefile-linux.mk
endif

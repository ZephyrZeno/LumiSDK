# LumiSDK — liblumiapp build
# Copyright 2026 Lumi Team. Apache-2.0

CC      ?= gcc
AR      ?= ar
CFLAGS  ?= -Wall -Wextra -O2 -fPIC -std=c11
INCLUDES = -Iinclude

SRC_DIR  = src
OBJ_DIR  = build
LIB_NAME = liblumiapp

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

PREFIX  ?= /usr/local
LIBDIR  ?= $(PREFIX)/lib
INCDIR  ?= $(PREFIX)/include

.PHONY: all clean install uninstall shared static test

all: shared static

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

shared: $(OBJS)
	$(CC) -shared -o $(OBJ_DIR)/$(LIB_NAME).so $(OBJS)

static: $(OBJS)
	$(AR) rcs $(OBJ_DIR)/$(LIB_NAME).a $(OBJS)

install: shared static
	install -d $(LIBDIR) $(INCDIR)
	install -m 644 $(OBJ_DIR)/$(LIB_NAME).so $(LIBDIR)/
	install -m 644 $(OBJ_DIR)/$(LIB_NAME).a  $(LIBDIR)/
	install -m 644 include/lumiapp.h $(INCDIR)/

uninstall:
	rm -f $(LIBDIR)/$(LIB_NAME).so $(LIBDIR)/$(LIB_NAME).a
	rm -f $(INCDIR)/lumiapp.h

test: static
	$(CC) $(CFLAGS) $(INCLUDES) -o $(OBJ_DIR)/test_sdk ../tests/test_sdk.c -L$(OBJ_DIR) -llumiapp
	./$(OBJ_DIR)/test_sdk

clean:
	rm -rf $(OBJ_DIR)

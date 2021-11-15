CC = cc
AR = ar
NAME = cafe
GL_API ?= 32
CFLAGS = -Wall -std=c99
LDFLAGS =

TARGET ?= OSX

MENU_BASE_DIR = ..

MENU = tea latte coffee mocha
MENU_FOLDERS = $(MENU:%=$(MENU_BASE_DIR)/%/)
MENU_FILES = $(join $(MENU_FOLDERS),$(MENU))

INCLUDE = -I. -Iexternal -Iexternal/lua/src -I$(MENU_BASE_DIR)/mocha/external
INCLUDE += $(addprefix -I,$(MENU_FOLDERS))

SRC = cafe.c $(MENU_FILES:%=%.c)
# SRC += $(wildcard external/lua/src/*.c) 
OBJS = $(SRC:.c=.o)
OUT = $(NAME)

LIBNAME = lib$(NAME)
SLIBNAME = $(LIBNAME).a
DLIBNAME = $(LIBNAME).so

#include cross/Makefile.$(TARGET)
ifeq ($(OS),Windows_NT)
        CFLAGS += -D_WIN32
        LDFLAGS += -mwindows -lopengl32
else
        UNAME_S = $(shell uname -s)
        ifeq ($(UNAME_S),Darwin)
                LDFLAGS += -framework OpenGL
        endif
        ifeq ($(UNAME_S),Linux)
                LDFLAGS += -lGL
        endif
endif

LDFLAGS += -L. -lcafe
LDFLAGS += -lm -lSDL2 -ldl

.PHONY: all build

build: $(OUT)
all: $(OUT) $(SLIBNAME)

$(OUT): main.c $(SLIBNAME)
	$(CC) main.c $(CFLAGS) -o $@ $(LDFLAGS) $(INCLUDE)

$(SLIBNAME): $(OBJS)
	$(AR) rcs $@ $^

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS) $(INCLUDE)

clean:
	rm -f $(OBJS)
	rm -f $(SLIBNAME)
	rm -f $(CLEAR_FILES)
	rm -f $(OUT)


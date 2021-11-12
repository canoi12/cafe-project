CC = cc
AR = ar
NAME = cafe
GL_API ?= 32
CFLAGS = -Wall -std=c99
LDFLAGS =

TARGET ?= OSX

MENU_BASE_DIR = ..

MENU = tea
MENU_FOLDERS = $(MENU:%=$(MENU_BASE_DIR)/%/)
MENU_FILES = $(join $(MENU_FOLDERS),$(MENU))

INCLUDE = -I. -Iexternal -Iexternal/lua/src -Imenu/mocha/external
INCLUDE += $(addprefix -I,$(MENU_FOLDERS))

SRC = cafe.c $(MENU_FILES:%=%.c)
# SRC += $(wildcard external/lua/src/*.c) 
OBJS = $(SRC:.c=.o)
OUT = $(NAME)

LIBNAME = lib$(NAME)
SLIBNAME = $(LIBNAME).a
DLIBNAME = $(LIBNAME).so

#include cross/Makefile.$(TARGET)
LDFLAGS += -framework OpenGL -lSDL2

LDFLAGS += -L. -lcafe

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


NAME = latte
SRC = latte.c
MAIN = main.c

CC = cc
AR = ar

CFLAGS = -Wall -std=c89
LFLAGS =

TARGET = 

LIBNAME = lib$(NAME)
SLIBNAME = $(LIBNAME).a
DLIBNAME = $(LIBNAME).so

INCLUDE =

OBJ = $(SRC:%.c=%.o)
DOBJ = $(SRC:%.c=%.do)

OUT = $(NAME)

.PHONY: all build
.SECONDARY: $(OBJ) $(DOBJ)

build: $(OUT)

all: $(SLIBNAME) $(DLIBNAME) $(OUT)

$(OUT): $(MAIN) $(SLIBNAME) $(NAME).h
	@echo "********************************************************"
	@echo "** COMPILING $@"
	@echo "********************************************************"
	$(CC) $(MAIN) -o $@ $(INCLUDE) $(CFLAGS) -L. -l$(NAME) $(LFLAGS)
	@echo ""

$(SLIBNAME): $(OBJ)
	@echo "********************************************************"
	@echo "** CREATING $@"
	@echo "********************************************************"
	$(AR) rcs $@ $(OBJ)
	@echo ""

$(DLIBNAME): $(DOBJ)
	@echo "********************************************************"
	@echo "** CREATING $@"
	@echo "********************************************************"
	$(CC) -shared -o $@ $(DOBJ) $(INCLUDE) $(CFLAGS)
	@echo ""

%.o: %.c
	@echo "********************************************************"
	@echo "** $(SLIBNAME): COMPILING SOURCE $<"
	@echo "********************************************************"
	$(CC) -c $< -o $@ $(INCLUDE) $(CFLAGS)

%.do: %.c
	@echo "********************************************************"
	@echo "** $(DLIBNAME): COMPILING SOURCE $<"
	@echo "********************************************************"
	$(CC) -c $< -o $@ -fPIC $(INCLUDE) $(CFLAGS)

clean:
	rm -rf $(OUT)
	rm -rf $(DLIBNAME) $(SLIBNAME)
	rm -rf $(OBJ) $(DOBJ)

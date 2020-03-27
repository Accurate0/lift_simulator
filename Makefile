CC=clang
CFLAGS=-Wall -std=c99 -Wextra -pedantic -g
CFLAGS+=-Wformat=2 -Wswitch-default -Wswitch-enum
CFLAGS+=-Wpointer-arith -Wbad-function-cast
CFLAGS+=-Wstrict-overflow=5 -Wstrict-prototypes
CFLAGS+=-Winline -Wundef -Wnested-externs
CFLAGS+=-Wcast-qual -Wshadow -Wunreachable-code
CFLAGS+=-Wfloat-equal -Wstrict-aliasing
CFLAGS+=-Wredundant-decls -Wold-style-definition
CFLAGS+=-ggdb3 -O0 -fno-omit-frame-pointer
CFLAGS+=-fno-common -Wdouble-promotion -Wcast-align
CFLAGS+=-Winit-self
CFLAGS+=-fsanitize=unsigned-integer-overflow,nullability,float-divide-by-zero
LDFLAGS=-pthread
EXEC_A=lift_sim_A
EXEC_B=lift_sim_B

SRC_DIR=src
OBJ_DIR=obj

SRC=$(wildcard $(SRC_DIR)/*.c)
OBJ=$(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

.PHONY: all clean

ifndef VALGRIND
CFLAGS+=-fsanitize=address,undefined
LDFLAGS+=-fsanitize=address -lubsan
endif

# CFLAGS+=-fsanitize=address -fsanitize-address-use-after-scope

ifdef DEBUG
CFLAGS+=-D DEBUG
DEBUG: clean $(EXEC_A)
endif

all: $(EXEC_A)

$(EXEC_A): $(OBJ)
	$(CC) $^ -o $@ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir $@

clean:
	$(RM) $(OBJ)

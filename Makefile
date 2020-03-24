CC=clang
CFLAGS=-Wall -std=c99 -Wextra -pedantic -g
LDFLAGS=-pthread
EXEC_A=lift_sim_A
EXEC_B=lift_sim_B

SRC_DIR=src
OBJ_DIR=obj

SRC=$(wildcard $(SRC_DIR)/*.c)
OBJ=$(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

.PHONY: all clean

ifdef TSAN
CFLAGS+=-fsanitize=thread
LDFLAGS+=-fsanitize=thread
TSAN: clean $(EXEC_A)
endif

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

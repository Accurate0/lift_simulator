CC=clang
CFLAGS=-Wall -std=c99 -Wextra -pedantic -ggdb3 -O0
CFLAGS+=-Isrc

LDFLAGS_A=-lpthread $(LDFLAGS)
LDFLAGS_B=-lrt $(LDFLAGS)

EXEC_A=lift_sim_A
EXEC_B=lift_sim_B

SRC_DIR=src
OBJ_DIR=obj

COMMON_SRC_DIR=$(SRC_DIR)/common
THREAD_SRC_DIR=$(SRC_DIR)/thread
PROCESS_SRC_DIR=$(SRC_DIR)/process

COMMON_SRC=$(wildcard $(COMMON_SRC_DIR)/*.c)
COMMON_OBJ=$(COMMON_SRC:$(COMMON_SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

THREAD_SRC=$(wildcard $(THREAD_SRC_DIR)/*.c)
THREAD_OBJ=$(THREAD_SRC:$(THREAD_SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

PROCESS_SRC=$(wildcard $(PROCESS_SRC_DIR)/*.c)
PROCESS_OBJ=$(PROCESS_SRC:$(PROCESS_SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

.PHONY: all clean

ifdef TSAN
CFLAGS+=-fsanitize=thread
LDFLAGS+=-fsanitize=thread
endif

ifdef ASAN
CFLAGS+=-fsanitize=address,unsigned-integer-overflow,nullability,float-divide-by-zero,undefined
CFLAGS+=-fsanitize-address-use-after-scope
LDFLAGS+=-fsanitize=address -lubsan
endif

ifdef DEBUG
CFLAGS+=-D DEBUG
DEBUG: clean all
endif

# CFLAGS+=-fsanitize=address

all: $(EXEC_A) $(EXEC_B)

# Filter out the opposing main
$(EXEC_A): $(COMMON_OBJ) $(THREAD_OBJ)
	$(CC) $^ -o $@ $(LDFLAGS_A)

$(EXEC_B): $(COMMON_OBJ) $(PROCESS_OBJ)
	$(CC) $^ -o $@ $(LDFLAGS_B)

$(OBJ_DIR)/%.o: $(THREAD_SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(PROCESS_SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(COMMON_SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir $@

clean:
	$(RM) $(EXEC_A) $(EXEC_B) $(COMMON_OBJ) $(THREAD_OBJ) $(PROCESS_OBJ)

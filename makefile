SERVER_SRC_DIR := src/server
CLIENT_SRC_DIR := src/client
SERVER_OBJ_DIR := obj/server
CLIENT_OBJ_DIR := obj/client
BIN_DIR := .

SERVER_SRC := $(wildcard $(SERVER_SRC_DIR)/*.c)
CLIENT_SRC := $(wildcard $(CLIENT_SRC_DIR)/*.c)

SERVER_OBJ := $(SERVER_SRC:$(SERVER_SRC_DIR)/%.c=$(SERVER_OBJ_DIR)/%.o)
CLIENT_OBJ := $(CLIENT_SRC:$(CLIENT_SRC_DIR)/%.c=$(CLIENT_OBJ_DIR)/%.o)

SERVER_EXE := server
CLIENT_EXE := client

CPPFLAGS := -Iinclude -MMD -MP
CFLAGS   := -Wall -lpthread
LDFLAGS  := -Llib
LDLIBS   := -lm
CC       := gcc 

.PHONY: all clean

all: $(SERVER_EXE) $(CLIENT_EXE)

$(SERVER_EXE): $(SERVER_OBJ) | $(BIN_DIR)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(CLIENT_EXE): $(CLIENT_OBJ) | $(BIN_DIR)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(SERVER_OBJ_DIR)/%.o: $(SERVER_SRC_DIR)/%.c | $(SERVER_OBJ_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(CLIENT_OBJ_DIR)/%.o: $(CLIENT_SRC_DIR)/%.c | $(CLIENT_OBJ_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(SERVER_OBJ_DIR):
	mkdir -p $@

$(CLIENT_OBJ_DIR):
	mkdir -p $@

clean:
	@$(RM) -rv $(SERVER_OBJ_DIR) $(CLIENT_OBJ_DIR)

-include $(SERVER_OBJ:.o=.d) $(CLIENT_OBJ:.o=.d)

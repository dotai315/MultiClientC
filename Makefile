TARGET = server 

CC = gcc
CFLAGS = -g -Wall -lpthread

SRC = src
SRCS = $(wildcard $(SRC)/*.c)

OBJ = obj
OBJS = $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SRCS))

INC = inc
INCLUDES = -I$(INC)

BIN_DIR = bin
BIN = $(BIN_DIR)/$(TARGET)

all: $(BIN)

$(BIN): $(OBJS) $(OBJ)
	$(CC) $(CFLAGS) $(OBJS) -o $@

$(OBJ)/%.o: $(SRC)/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OBJ):
	mkdir -p $@

clean:
	rm -rf $(BIN_DIR)/* $(OBJ)/*

edit:
	vim -O $(SRC)/*.c $(INC)/*.h

dir:
	mkdir inc src obj bin 
	touch $(INC)/main.h $(SRC)/main.c

create-project:
	mkdir $(TARGET) 
	cp ./Makefile ./$(TARGET)/

delete-project:
	rm -rf inc src obj bin

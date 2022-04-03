CC = gcc

INCLUDE = ./inc
SOURCE = ./src
BUILD = ./build
BINARY = ./bin

API = main.o lexer.o parser.o

all: $(API)
	$(CC) $(addprefix $(BUILD)/, $^) -o $(BINARY)/con

$(API):
	$(CC) -c $(SOURCE)/$(basename $@).c -o $(BUILD)/$@ -I $(INCLUDE) -I $(SOURCE)
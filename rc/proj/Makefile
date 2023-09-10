
# ---------------------------------------------- #
#              Global Makefile.                  #
#     calls 'make' on the client and GS dirs     #
#  Group 36 (Filipa Magalhaes & Valentim Santos) #
# ---------------------------------------------- #


CC = gcc

CFLAGS :=
LDFLAGS :=

COMMON_DIR = common

GAMES_DIR = games
SCORES_DIR = scores


# List of the dirs to include in the makefile.
INCLUDE_DIRS :=  $(COMMON_DIR)
# Adds the -I prefix to all the included dirs (needed).
INCLUDES = $(addprefix -I, $(INCLUDE_DIRS))

# Gets all the '.c' and '.h' files needed.
CLIENT_SOURCE := player.c
SERVER_SOURCE := GS.c
HEADERS := $(wildcard $(COMMON_DIR)/*h)

# Gets the names of the executables needed (ex: player.c -> player).
CLIENT_EXEC := $(CLIENT_SOURCE:.c=)
SERVER_EXEC := $(SERVER_SOURCE:.c=)


JPG_FILES := $(wildcard *.jpg)
PNG_FILES := $(wildcard *.png)
JPEG_FILES := $(wildcard *.jpeg)
SVG_FILES := $(wildcard *.svg)


HINT_FILES := $(JPG_FILES) $(PNG_FILES) $(JPEG_FILES) $(SVG_FILES)


# Keeps make from doing something with fles with these names.
.PHONY: all clean

all: $(CLIENT_EXEC) $(SERVER_EXEC)


$(CLIENT_EXEC): $(CLIENT_SOURCE) $(HEADERS)
	$(CC) $< $(LDFLAGS) $(CFLAGS) -o $@


$(SERVER_EXEC): $(SERVER_SOURCE) $(HEADERS)
	$(CC) $< $(LDFLAGS) $(CFLAGS) -o $@


clean:
	rm -rf $(CLIENT_EXEC) $(SERVER_EXEC) $(HINT_FILES) $(GAMES_DIR)/* $(SCORES_DIR)/*

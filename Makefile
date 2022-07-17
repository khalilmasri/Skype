SRC=src
SERVERSRC=$(SRC)/server
OBJ=obj
SERVEROBJ=$(OBJ)/server
BIN=bin
TMP=.tmp
CLIENTFILE = client

CFLAGS += -Wfatal-errors -w -pthread -I ./include
CC=g++ -std=c++11
SERVERTARGET=server
RM=rm -rf
MKDR=mkdir -p

OBJDIR := $(shell $(MKDR) $(OBJ) $(TMP) $(CLIENTFILE) $(SERVEROBJ))

SERVERSRCS=$(wildcard $(SERVERSRC)/*.cpp)
SERVEROBJS=$(patsubst $(SERVERSRC)/%.cpp, $(SERVEROBJ)/%.o, $(SERVERSRCS))

# server

all: $(SERVERTARGET)

$(SERVERTARGET): $(SERVEROBJS)
		$(CC) -o $(SERVERTARGET) $(SERVEROBJS) $(CFLAGS)

$(SERVEROBJ)/%.o: $(SERVERSRC)/%.cpp
		${CC} ${CFLAGS} -c $< -o $@

clean:
	$(RM) $(OBJ)/*/*.o 
	$(RM) $(TMP)/*

fclean: clean
	$(RM) $(SERVERTARGET)
	$(RM) $(TMP)
	$(RM) $(OBJ)

re: 	clean
	$(MAKE) all

.PHONY: all clean fclean re


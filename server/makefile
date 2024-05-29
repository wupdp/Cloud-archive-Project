CC = g++
CFLAGS_COMMON = -std=c++11 -Wall -Os -fPIE
CFLAGS_DEBUG = $(CFLAGS_COMMON) -g -ggdb
CFLAGS_RELEASE = $(CFLAGS_COMMON) -Werror

LDFLAGS = -lssl -lcrypto

SRCS = src/authentification.cpp src/main.cpp src/server.cpp src/utilities.cpp src/ftp_commands.cpp
OBJS = $(SRCS:.cpp=.o)
EXEC = ftpserver

.PHONY: all clean

all: $(EXEC)

$(EXEC): $(OBJS)
ifeq ($(MODE),debug)
	$(CC) $(CFLAGS_DEBUG) $(OBJS) -o $(EXEC) $(LDFLAGS)
else
	$(CC) $(CFLAGS_RELEASE) $(OBJS) -o $(EXEC) $(LDFLAGS)
endif

%.o: %.cpp include/authentification.h include/server.h include/utilities.h include/ftp_commands.h
	$(CC) $(CFLAGS_COMMON) -c $< -o $@

clean:
	rm -f $(OBJS) $(EXEC)
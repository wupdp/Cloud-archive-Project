CC = g++
CFLAGS_COMMON = -std=c++11 -Wall -g -Os
CFLAGS_DEBUG = $(CFLAGS_COMMON) -g -ggdb
CFLAGS_RELEASE = $(CFLAGS_COMMON) -Werror

SRCS = main.cpp client.cpp
OBJS = $(SRCS:.cpp=.o)
EXEC = ftclient

.PHONY: all clean

all: $(EXEC)

$(EXEC): $(OBJS)
ifeq ($(MODE),debug)
	$(CC) $(CFLAGS_DEBUG) $(OBJS) -o $(EXEC)
else
	$(CC) $(CFLAGS_RELEASE) $(OBJS) -o $(EXEC)
endif

%.o: %.cpp client.h
	$(CC) $(CFLAGS_COMMON) -c $< -o $@

clean:
	rm -f $(OBJS) $(EXEC)

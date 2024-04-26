CC = g++
CFLAGS = -std=c++11 -Wall
LDFLAGS = -lssl -lcrypto

SRCS = src/main.cpp
OBJS = $(SRCS:.cpp=.o)
EXEC = MyProject

.PHONY: all clean

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(EXEC) $(LDFLAGS)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(EXEC)
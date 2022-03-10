CC = g++
FLAGS = -std=c++11 -Wall
RM = rm




all: Client.out Server.out


Client.out: Client.cpp
	$(CC) $(FLAGS) Client.cpp -o Client.out

Server.out: Server.cpp
	$(CC) $(FLAGS) Server.cpp -o Server.out


.PHONY: clean
clean:
	$(RM) *.out	
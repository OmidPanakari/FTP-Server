CC = g++
FLAGS = -std=c++11 -Wall
RM = rm




all: Client.out Server.out Logger.o


Client.out: Client.cpp
	$(CC) $(FLAGS) Client.cpp -o Client.out

Server.out: Server.cpp Logger.o
	$(CC) $(FLAGS) Server.cpp Logger.o -o Server.out

Logger.o: Logger.cpp Logger.hpp
	$(CC) $(FLAGS) Logger.cpp -c


.PHONY: clean
clean:
	$(RM) *.out	*.o
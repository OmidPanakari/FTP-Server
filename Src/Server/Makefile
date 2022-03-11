CC = g++
FLAGS = -std=c++11 -Wall
RM = rm




all: Server.out Logger.o JsonSerializer.o


Server.out: Server.cpp Logger.o JsonSerializer.o
	$(CC) $(FLAGS) Server.cpp Logger.o JsonSerializer.o -o Server.out

Logger.o: Logger.cpp Logger.hpp
	$(CC) $(FLAGS) Logger.cpp -c

JsonSerializer.o: JsonSerializer.cpp JsonSerializer.hpp
	$(CC) $(FLAGS) JsonSerializer.cpp -c




.PHONY: clean
clean:
	$(RM) *.out	*.o
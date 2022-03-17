CC = g++
FLAGS = -std=c++17 -Wall
RM = rm




all: Server.out Logger.o JsonSerializer.o


Server.out: Server.cpp Logger.o JsonSerializer.o ServerCore.o
	$(CC) $(FLAGS) Server.cpp Logger.o JsonSerializer.o ServerCore.o -o Server.out

ServerCore.o: ServerCore.cpp ServerCore.hpp JsonSerializer.o
	$(CC) $(FLAGS) ServerCore.cpp JsonSerializer.o -c

Logger.o: Logger.cpp Logger.hpp
	$(CC) $(FLAGS) Logger.cpp -c

JsonSerializer.o: JsonSerializer.cpp JsonSerializer.hpp
	$(CC) $(FLAGS) JsonSerializer.cpp -c




.PHONY: clean
clean:
	$(RM) *.out	*.o
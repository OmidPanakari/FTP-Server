CC = g++
FLAGS = -std=c++17 -Wall
RM = rm




all: server.out


server.out: main.o ServerAPI.o ServerCore.o Logger.o JsonSerializer.o Utility.o
	$(CC) $(FLAGS) main.o Logger.o JsonSerializer.o ServerCore.o ServerAPI.o Utility.o -o Server.out

main.o: main.cpp ServerAPI.hpp ServerCore.hpp
	$(CC) $(FLAGS) main.cpp -c

ServerAPI.o: ServerAPI.cpp ServerAPI.hpp Logger.hpp JsonSerializer.hpp ServerAPI.hpp Utility.hpp
	$(CC) $(FLAGS) ServerAPI.cpp -c

ServerCore.o: ServerCore.cpp ServerCore.hpp JsonSerializer.hpp Logger.hpp Utility.hpp
	$(CC) $(FLAGS) ServerCore.cpp -c

Logger.o: Logger.cpp Logger.hpp
	$(CC) $(FLAGS) Logger.cpp -c

JsonSerializer.o: JsonSerializer.cpp JsonSerializer.hpp
	$(CC) $(FLAGS) JsonSerializer.cpp -c

Utility.o: Utility.cpp Utility.hpp
	$(CC) $(FLAGS) Utility.cpp -c




.PHONY: clean
clean:
	$(RM) *.out	*.o
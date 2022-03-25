CC = g++
FLAGS = -std=c++17 -Wall
RM = rm




all: Client.out


Client.out: main.o Client.o JsonSerializer.o Utility.o
	$(CC) $(FLAGS) main.o Client.o JsonSerializer.o Utility.o -o Client.out

main.o: main.cpp Client.hpp
	$(CC) $(FLAGS) main.cpp -c

Client.o: Client.cpp JsonSerializer.hpp Utility.hpp
	$(CC) $(FLAGS) Client.cpp -c

JsonSerializer.o: JsonSerializer.cpp JsonSerializer.hpp
	$(CC) $(FLAGS) JsonSerializer.cpp -c

Utility.o: Utility.cpp Utility.hpp
	$(CC) $(FLAGS) Utility.cpp -c




.PHONY: clean
clean:
	$(RM) *.out	*.o
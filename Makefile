INCLUDE=-I glfw/include -I sqlite3
LIBRARY=-L glfw/lib/cocoa
FLAGS=-std=c99 -O3

all: main

run: all
	./main

clean:
	rm *.o

main: main.o util.o
	gcc $(FLAGS) main.o util.o -o main $(LIBRARY) -lglfw -framework OpenGL

main.o: main.c
	gcc $(FLAGS) $(INCLUDE) -c -o main.o main.c

util.o: util.c util.h
	gcc $(FLAGS) $(INCLUDE) -c -o util.o util.c

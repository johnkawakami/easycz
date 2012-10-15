
FLAGS := `pkg-config --cflags --libs gtk+-2.0` \
  -lyaml -ggdb 

all: main.o config.o
	gcc -o easycz main.o config.o $(FLAGS)
	
main.o: main.c globals.h main.h
	gcc -c main.c $(FLAGS)

config.o: config.c config.h globals.h
	gcc -c config.c $(FLAGS)

test: test.c
	gcc test.c -o test $(FLAGS)

clean:
	rm *.o easycz

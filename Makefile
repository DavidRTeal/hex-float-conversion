CC = gcc

CFLAGS = -Wall -Wshadow -Wunreachable-code -Wredundant-decls -Wmissing-declarations -Wold-style-definition -Wmissing-prototypes -Wdeclaration-after-statement -Wextra #-Werror -Wpedantic

all: hex-2-float float-2-hex

hex-2-float: hex-2-float.o
	$(CC) $(CFLAGS) -o hex-2-float hex-2-float.o -lm

hex-2-float.o: hex-2-float.c
	$(CC) $(CFLAGS) -c hex-2-float.c 

float-2-hex: float-2-hex.o
	$(CC) $(CFLAGS) -o float-2-hex float-2-hex.o -lm

float-2-hex.o: float-2-hex.c
	$(CC) $(CFLAGS) -c float-2-hex.c 

clean:
	rm -f hex-2-float float-2-hex *.o *~

tar:
	tar cvfz lab5-${LOGNAME}.tar.gz *.[ch] ?akefile

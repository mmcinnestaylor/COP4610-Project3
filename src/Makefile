CC = gcc -g
RM = rm -f fattyshell *.o

fattyshell: fattyshell.o
	$(CC) -o fattyshell fattyshell.o

fattyshell.o: fattyshell.c fattyshell.h
	$(CC) -w -c fattyshell.c	

clean: 
	$(RM)

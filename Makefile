client: client.o
	gcc client.o -o client

run: server.c ph7.c php_parser.c
	gcc -W -Wall -O6 -o server server.c ph7.c
	./server

clean:
	-rm -f *.o
	-rm -f server
	-rm -f client
all: server client

server: instagrapd.c
	gcc -g -o server instagrapd.c

client: submitter.c
	gcc -o client submitter.c

clean:
	rm -rf client server

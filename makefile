all: client server

client: hangman_client.c
	gcc -o hangman_client hangman_client.c

server: hangman_server.c
	gcc -o hangman_server hangman_server.c

clean: ; rm hangman_client ; hangman_server
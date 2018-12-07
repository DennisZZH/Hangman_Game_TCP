make all:
    g++ hangman_client.c
    g++ hangman_server.c

make clean:
    rm hangman_client
    rm hangman_server

make client:
	g++ hangman_client.c

make server:
	g++ hangman_server
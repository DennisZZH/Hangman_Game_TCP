make all:
    g++ hangman_client.c hangman_client
    g++ hangman_server.c hangman_server

make clean:
    rm hangman_client
    rm hangman_server

make client:
	g++ hangman_client.c hangman_client

make server:
	g++ hangman_server.c hangman_server
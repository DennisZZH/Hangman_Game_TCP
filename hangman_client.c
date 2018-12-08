//Author: Zihao Zhang & Jiajun Wan
//Date: 12.7

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

void error(const char *msg) {
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[]) {
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char rcv_msg[256];
    char msg[256];
    char Choice[3];
    char Guess[256];
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
        error("ERROR connecting");

    printf("Ready to start game? (y/n): ");
    fgets(Choice, 3, stdin);
    if (Choice[0] == 'n') {printf("You choose n. Terminate the client.\n"); close(sockfd); return 0;}
    printf("\n");

    bzero(msg, 256);
	msg[0] = '0';
	n = write(sockfd, msg, strlen(msg));    //send 0 to server
    if (n < 0) 
         error("ERROR writing to socket");
    
    bzero(rcv_msg, 256);
    n = read(sockfd, rcv_msg, 255);  //read from server
    if (n < 0) 
         error("ERROR reading from socket");
    // printf("%s", rcv_msg);  //test

    if(rcv_msg[0] > 0 && rcv_msg[1] == 's' && rcv_msg[2] == 'e') {
        printf("server-overloaded\n");
        close(sockfd);
        return 0;
    }

    int WordLength = rcv_msg[1] - '0';   //print the Word (first with _______)
    //printf("%d", WordLength);   //test
    for (int i = 0; i < WordLength; i++) {
        printf("%c", rcv_msg[i+3]);
    }
    printf("\n");

    int NumIncorrect = rcv_msg[2] - '0'; //print the incorrect guesses if there are
    printf("Incorrect Guesses: ");
    if (NumIncorrect != 0) {
        printf("%c", rcv_msg[WordLength+3]);
        for (int i = 1; i < NumIncorrect; i++) {
            printf(" ,%c", rcv_msg[i+WordLength+3]);
        }
    }
    printf("\n\n");

    int End = 0;
    while (1 - End) {
        printf("Letter to guess: ");
		bzero(Guess, 256);
		fgets(Guess, 255, stdin);
		int GuessLetter = tolower(Guess[0]);
		while(strlen(Guess) != 2 || GuessLetter < 97 || GuessLetter > 122) {
			printf("Error! Please guess one letter.\n");
			printf("letter to guess: ");
			bzero(Guess, 256);
			fgets(Guess, 255, stdin);
			GuessLetter = tolower(Guess[0]);
		}
        
		bzero(msg, 256);
		msg[0] = '1';
		msg[1] = Guess[0];
		n = write(sockfd, msg, strlen(msg));    //send the guess to server
		bzero(rcv_msg, 256);
		n = read(sockfd, rcv_msg, 255); //read the reply

        if(rcv_msg[0] != '0') { //if Msg flag is not 0
            int WordLength = rcv_msg[0] - '0';   //print the final message to end the game
            for (int i = 0; i < WordLength; i++) {
                printf("%c", rcv_msg[i+1]);
            }
            printf("\n");
			End = 1;
		}
		else {
            int WordLength = rcv_msg[1] - '0';   //print the Word (first with _______)
            //printf("%d", WordLength);   //test
            for (int i = 0; i < WordLength; i++) {
                printf("%c", rcv_msg[i+3]);
            }
            printf("\n");

            int NumIncorrect = rcv_msg[2] - '0'; //print the incorrect guesses if there are
            printf("Incorrect Guesses: ");
            if (NumIncorrect != 0) {
                printf("%c", rcv_msg[WordLength+3]);
                for (int i = 1; i < NumIncorrect; i++) {
                    printf(", %c", rcv_msg[i+WordLength+3]);
                }
            }
            printf("\n\n");
		}
	}
    close(sockfd);
    return 0;
}
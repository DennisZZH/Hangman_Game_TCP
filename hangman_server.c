//Author: Zihao Zhang
//Date: 12.7
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

int sockfd, newsockfd, portno;
int n;
socklen_t clilen;
struct sockaddr_in serv_addr, cli_addr;

char clientMessage[2];                 // buffer sused to receive user message
char serverMessage[1024];

char[6] gameWord = "dennis";		// hardcoded game word
char[6] currentWord = "______";
char[6] WrongGuess = "";
int failureCount = 0;
int successCount = 0;
bool isOverTrue= false;

int charSuccessCount;
char userGuess;

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
       error("ERROR opening socket");

    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
        error("ERROR on binding");

    while(1){

        listen(sockfd,5);
        clilen = sizeof(cli_addr);
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if (newsockfd < 0) 
            error("ERROR on accept");

        n = recv(newsockfd,clientMessage,2,0);
        if (n < 0) 
            error("ERROR reading from socket");
        else if(n == 0){                    // the first empty message received, game starts now
        
            serverMessage[0] = '0';
            serverMessage[1] = strlen(gameWord) + '0';
            serverMessage[2] = failureCount + '0';
            for(int i = 0; i < strlen(currentWord); i++){
                serverMessage[i+3] = currentWord[i];
            }
            send(newsockfd, serverMessage, messageLength,0);
            
            startGame();
        }
        close(newsockfd);

    }
    close(sockfd);
    return 0;
}

void startGame(){

    while(isOverTrue == false){

        bzero(clientMessage);
        n = recv(newsockfd, clientMessage, 2, 0);
        if (n < 0) 
             error("ERROR reading from socket");

        userGuess = clientMessage[1];
        charSuccessCount = 0;
        for(int i = 0; i < strlen(gameWord); i++){
            if(userGuess == gameWord[i]){
                currentWord[i] = gameWord[i];
                charSuccessCount++;
            }
        }

        if(charSuccessCount == 0){
            WrongGuess[failureCount] = userGuess;                
            failureCount++;
        }else{
            successCount++;
        }

        bzero(serverMessage);

        if(successCount == strlen(gameWord)){
            isOverTrue = true;
            serverMessage[0] = '8';
            serverMessage += "You Win!";
            
        }else if(failureCount >= 6){
            isOverTrue = true;
            serverMessage[0] = '9';
            serverMessage += "You Lost!";      
        }else{
            
            serverMessage[0] = '0';
            serverMessage[1] = strlen(gameWord) + '0';
            serverMessage[2] = failureCount + '0';
            for(int i = 0; i < strlen(currentWord); i++){
                serverMessage[i+3] = currentWord[i];
            }
            for(int j = 0; j < strlen(WrongGuess); j++){
                serverMessage[j+3+strlen(currentWord)] = WrongGuess[j];
            }
        }

        send(newsockfd, serverMessage, strlen(serverMessage),0);           
    }
}


        







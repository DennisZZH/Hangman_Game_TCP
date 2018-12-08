//Author: Zihao Zhang & Jiajun Wan
//Date: 12.7

#include <sys/types.h>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<sys/ioctl.h>
#include<sys/poll.h>
#include<sys/socket.h>
#include<sys/time.h>
#include<netinet/in.h>
#include<errno.h>
#include<string.h>
#include<time.h>

int sockfd, newsockfd, portno;
int n;
socklen_t clilen;
struct sockaddr_in serv_addr, cli_addr;

char clientMessage[2];                 // buffer sused to receive user message
char serverMessage[1024];

char gameWord[8] = "";
char currentWord[8] = "________";
char WrongGuess[6] = "";
int failureCount = 0;
int successCount = 0;
int isOverTrue= 0;

int charSuccessCount;
char userGuess;

char Win[8] = "You Win!";
char Lose[9] = "You Lost!";

FILE * input;
char wordList[15][8];
int wordnum;
int randomNum;
int RandomWordLength;

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

void getWord(){
    wordnum = 0;
    input = fopen("hangman_words.txt", "r");
    while(fscanf(input, "%s", wordList[wordnum]) == 1) {
      wordnum++;
      printf("%s\n", wordList[wordnum]);
    }

    srand(time(0));   // Initialization, should only be called once // Returns a pseudo-random integer between 0 and RAND_MAX.
    
    randomNum = rand() % wordnum;
    
    RandomWordLength = sizeof(wordList[randomNum]);
    
    strcpy(gameWord, wordList[RandomWordLength]);

    printf("%s\n", gameWord);
}

void startGame(){

    while(isOverTrue == 0){

        bzero(clientMessage, strlen(clientMessage));

        //printf("Inside startGame; receiving user guess\n");
        n = recv(newsockfd, clientMessage, 2, 0);
        if (n < 0) 
             error("ERROR reading from socket");

        userGuess = clientMessage[1];
        //printf("userGuess is ");
        //printf("%c\n", userGuess);

        charSuccessCount = 0;
        for(int i = 0; i < strlen(gameWord); i++){
            if(userGuess == gameWord[i]){
                currentWord[i] = gameWord[i];
                charSuccessCount++;
            }
        }

        //printf("charSuccessCount is ");
        //printf("%d\n", charSuccessCount);

        if(charSuccessCount == 0){
            WrongGuess[failureCount] = userGuess;                
            failureCount++;
        }else{
            successCount+= charSuccessCount;
        }

        bzero(serverMessage, sizeof(serverMessage));

        if(successCount == strlen(gameWord)){
            //printf("in success case\n");
            isOverTrue = 1;
            serverMessage[0] = '8';
            for(int i = 0; i < sizeof(Win); i++){
                //printf("!!!!!!!!!!" );
                serverMessage[i+1] = Win[i];
            }
            
        }else if(failureCount >= 6){
            //printf("in failure case\n");
            isOverTrue = 1;
            serverMessage[0] = '9';
            for(int i = 0; i < sizeof(Lose); i++){
                 //printf("!!!!!!!!!!" );
                serverMessage[i+1] = Lose[i];
            } 

        }else{
            //printf("in keep playingh case\n");
            
            serverMessage[0] = '0';
            //printf("%c\n",serverMessage[0]);

            serverMessage[1] = strlen(gameWord) + '0';
            //printf("%c\n",serverMessage[1]);

            serverMessage[2] = failureCount + '0';
            //printf("%c\n",serverMessage[2]);

            for(int i = 0; i < strlen(gameWord); i++){
                serverMessage[i+3] = currentWord[i];
                //printf("%c", currentWord[i]);
            }
                //printf("\n");

            for(int j = 0; j < strlen(WrongGuess); j++){
                serverMessage[j+3+strlen(gameWord)] = WrongGuess[j];
                //printf("%c", WrongGuess[j]);
            }
                //printf("\n");
        }

        //printf("sending server message\n");
        //printf("%s\n", serverMessage);
        n = send(newsockfd, serverMessage, strlen(serverMessage),0);
        if (n < 0) 
            error("ERROR sending from server");
               
    }
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

    getWord();

    while(1){

        listen(sockfd,5);
        clilen = sizeof(cli_addr);
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if (newsockfd < 0) 
            error("ERROR on accept");

        n = recv(newsockfd,clientMessage,2,0);
        //printf("received initial consent from client\n");

        if (n < 0) 
            error("ERROR reading from socket");
        else if(n == 1){                    // the first empty message received, game starts now
        
            //printf("server preparing first message\n");

            serverMessage[0] = '0';
            //printf("%c\n",serverMessage[0]);

            serverMessage[1] = strlen(gameWord) + '0';
            //printf("%c\n",serverMessage[1]);

            serverMessage[2] = failureCount + '0';
            //printf("%c\n",serverMessage[2]);

            for(int i = 0; i < strlen(gameWord); i++){
                serverMessage[i+3] = currentWord[i];
            }
            
            //printf("server sending first message\n");
            send(newsockfd, serverMessage, strlen(serverMessage),0);
            
            //printf("starting game\n");
            startGame();
        }
        close(newsockfd);

    }
    close(sockfd);
    return 0;
}



        







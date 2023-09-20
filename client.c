#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "socket.h"

#define PORT 6060
#define SERVER_IP "127.0.0.1"

int main(){
    int index = 0;
    char message[1024];
    int clientSocketDescriptor = socket(AF_INET, SOCK_STREAM, 0);
    
    if(clientSocketDescriptor < 0){
        fprintf(stderr, "Falha ao criar socket\n");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in serverAddr;
    configureSocket(&serverAddr, PORT);
    
    if (connect(clientSocketDescriptor, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Erro ao conectar\n");
        exit(EXIT_FAILURE);
    }

    printf("Estou enviando números!\n");
    while(1){
        sprintf(message, "%d", index);
        if (send(clientSocketDescriptor, message, sizeof(message), 0) == -1) {
            perror("Erro ao enviar dados\n");
            exit(EXIT_FAILURE);
        }
        fprintf(stdout, "%d\n", index);
        index++;
        sleep(1);
    }

    close(clientSocketDescriptor);

    return 0;
}

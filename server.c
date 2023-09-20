#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/stat.h>
#include "socket.h"


#define PORT 6060
#define BACKLOG 5


int main(){
    int serverSocketDescriptor, clientSocketDescriptor;
    char buffer[1024];
    struct sockaddr_in serverAddr;                         // Variável que representa o endereço do socket do servidor
    struct sockaddr_in clientAddr;                         // Variável que representa o endereço do socket do cliente
    socklen_t clientAddrLen = sizeof(clientAddr);

    FILE *file;
    // Criacao do socket do servidor
    serverSocketDescriptor = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocketDescriptor == -1) {
        fprintf(stderr,"Erro ao criar o socket do servidor\n");
        exit(EXIT_FAILURE);
    }

    // Configurando e realizando o bind do socket com o endereço escolhido
    serverSocketDescriptor = bindServerSocket(serverSocketDescriptor, &serverAddr, PORT);

    // Deixa a porta ouvindo conexões
    listenConnections(serverSocketDescriptor, PORT, BACKLOG);

    // Espera pela conexao de um cliente
    clientSocketDescriptor = awaitClient(serverSocketDescriptor, &clientAddr, &clientAddrLen);

    fclose(file);
    close(clientSocketDescriptor);
    close(serverSocketDescriptor);

    return 0;
}


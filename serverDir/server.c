#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "../socket.h"
#include <sys/time.h>

#define PORT 6061
#define BACKLOG 5

char * receiveFileName(int socketDescriptor, char * fileName, size_t fileNameSize);
void receiveFile(int socketDescriptor, char * fileName);

int main(){
    int serverSocketDescriptor, clientSocketDescriptor;
    char fileName[256];
    struct sockaddr_in serverAddr;                         // Variável que representa o endereço do socket do servidor
    struct sockaddr_in clientAddr;                         // Variável que representa o endereço do socket do cliente
    socklen_t clientAddrLen = sizeof(clientAddr);
    FILE * file;

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
    receiveFileName(clientSocketDescriptor, fileName, sizeof(fileName));
    receiveFile(clientSocketDescriptor, fileName);

    close(clientSocketDescriptor);
    close(serverSocketDescriptor);

    return 0;
}

char * receiveFileName(int socketDescriptor, char * fileName, size_t fileNameSize){
    size_t bytesRead;

    bytesRead = recv(socketDescriptor, fileName, fileNameSize, 0);
    if (bytesRead == 0) {
        fprintf(stderr, "Erro ao receber o nome do arquivo\n");
        close(socketDescriptor);
    }
    fprintf(stdout, "FILE NAME: %s\n", fileName);
    return fileName;
}

void receiveFile(int socketDescriptor, char * fileName){
    char buffer[1024];
    struct timeval tv;
    ssize_t bytesRead;
    FILE  * file;

    if (fileName == NULL) {
        fprintf(stderr, "Arquivo sem nome!\n");
        return;
    }

    file = fopen("teste-recebido.txt", "wb");
    if (file == NULL) {
        fprintf(stderr, "Erro ao abrir o arquivo\n");
        close(socketDescriptor);
        exit(EXIT_FAILURE);
    }

    // pegar o horario de inicio
    time_t initTime = tv.tv_usec;
    while ((bytesRead = recv(socketDescriptor, buffer, sizeof(buffer), 0)) > 0){
        fwrite(buffer, 1, bytesRead, file);
        printf("%s\n", buffer);
        printf("BYTES: %zd\n", bytesRead);
    }

    // pegar o horario de fim
    time_t finishTime = tv.tv_usec;
    fprintf(stdout, "TEMPO PERCORRIDO: %ld.%.6ld\n",  (finishTime - initTime));
    fclose(file);
}

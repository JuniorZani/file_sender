#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "../socket.h"
#include <sys/time.h>

#define PORT 6061
#define BACKLOG 5
//#define FILE_NAME "teste-recebido.txt"
#define CHUNCK_SIZE (1024 * 1024 * 256)


char * receiveFileName(int socketDescriptor, char * fileName, size_t fileNameSize);
void receiveFile(int socketDescriptor, char * fileName);

int main(){
    int serverSocketDescriptor, clientSocketDescriptor;
    char fileName[256];
    struct sockaddr_in serverAddr;                         // Variável que representa o endereço do socket do servidor
    struct sockaddr_in clientAddr;                         // Variável que representa o endereço do socket do cliente
    socklen_t clientAddrLen = sizeof(clientAddr);
    FILE * file;
    int chunckSize = CHUNCK_SIZE;

    // Criacao do socket do servidor
    serverSocketDescriptor = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocketDescriptor == -1) {
        fprintf(stderr,"Erro ao criar o socket do servidor\n");
        exit(EXIT_FAILURE);
    }

//    if (setsockopt(serverSocketDescriptor, SOL_SOCKET, SO_RCVBUF, &chunckSize, chunckSize) < 0) {
//        fprintf(stderr, "Erro ao configurar o tamanho do buffer de recepção");
//        close(serverSocketDescriptor);
//        return 1;
//    }
    int optval = 1;
    setsockopt(serverSocketDescriptor, SOL_SOCKET, SO_REUSEADDR,(const void *) &optval, sizeof(int));

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
    char * buffer = malloc(CHUNCK_SIZE);
    struct timeval tv;
    struct timeval init_time, finish_time;
    ssize_t bytesRead;
    FILE  * file;

    if (fileName == NULL) {
        fprintf(stderr, "Arquivo sem nome!\n");
        return;
    }
    #ifdef FILE_NAME
        strcpy(fileName, FILE_NAME);
    #endif

    file = fopen(fileName, "wb");

    if (file == NULL) {
        fprintf(stderr, "Erro ao abrir o arquivo\n");
        close(socketDescriptor);
        exit(EXIT_FAILURE);
    }

    // pegar o horario de inicio
    gettimeofday(&init_time, NULL);
    while ((bytesRead = recv(socketDescriptor, buffer, CHUNCK_SIZE, 0)) > 0){
        fwrite(buffer, 1, bytesRead, file);
//        printf("%s\n", buffer);
        printf("BYTES: %zd\n", bytesRead);
        printf("KILOBYTES: %zd\n", (bytesRead/1024));
        printf("MEGABYTES: %zd\n\n", (bytesRead/(1024 * 1024)));
    }
//    while ((bytesRead = read(socketDescriptor, buffer, CHUNCK_SIZE)) > 0){
//        //fwrite(buffer, 1, bytesRead, file);
////        printf("%s\n", buffer);
//        printf("BYTES: %zd\n", bytesRead);
//        printf("KILOBYTES: %zd\n", (bytesRead/1024));
//        printf("MEGABYTES: %zd\n\n", (bytesRead/(1024 * 1024)));
//    }

    // pegar o horario de fim
    time_t finishTime = tv.tv_usec + tv.tv_sec;
    gettimeofday(&finish_time, NULL);
    fprintf(stdout, "TEMPO PERCORRIDO SEGUNDOS: %ld.%.6ld\n",  (finish_time.tv_sec - init_time.tv_sec));
    fclose(file);
}

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include "../socket.h"

#define PORT 6061
//#define SERVER_IP "192.168.1.102"
//#define SERVER_IP "192.168.1.103"
#define SERVER_IP "192.168.195.203"
//#define SERVER_IP "127.0.0.1"
//#define FILE_NAME "NormalSizeFile.bin"
#define FILE_NAME "BigFile.bin"
//#define CHUNCK_SIZE (524288)
#define CHUNCK_SIZE (1024 * 1024 * 256)

void sendNumbers(int socketDescriptor);
void sendFile(FILE * file, char * fileName, size_t fileNameSize, int socketDescriptor);

int main(){
    int clientSocketDescriptor = socket(AF_INET, SOCK_STREAM, 0);
    int chunckSize = CHUNCK_SIZE;
    char fileName[256];
    struct sockaddr_in serverAddr;

    FILE * file;

    if(clientSocketDescriptor < 0){
        fprintf(stderr, "Falha ao criar socket\n");
        exit(EXIT_FAILURE);
    }

    configureClientSocket(&serverAddr, PORT, SERVER_IP);
    
    if (connect(clientSocketDescriptor, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
        fprintf(stderr,"Erro ao conectar\n");
        exit(EXIT_FAILURE);
    }

    strcpy(fileName, FILE_NAME);
    file = fopen(fileName, "rb");
    if (file == NULL) {
        fprintf(stderr, "Erro ao abrir o arquivo: %d\n", errno);
        close(clientSocketDescriptor);
        exit(EXIT_FAILURE);
    }

    sendFile(file, fileName, strlen(fileName), clientSocketDescriptor);

    close(clientSocketDescriptor);

    return 0;
}

void sendFile(FILE * file, char * fileName, size_t fileNameSize, int socketDescriptor) {
    int i = 1;
    size_t bytesRead;
    char *buffer = malloc(CHUNCK_SIZE);

    printf("Estou enviando o arquivo!\n");

    // Enviando o nome do arquivo
    if (send(socketDescriptor, fileName, fileNameSize + 1, 0) == -1) {
        perror("Erro ao enviar o nome do arquivo\n");
        exit(EXIT_FAILURE);
    }

    // Enviando os dados do arquivo
    while ((bytesRead = fread(buffer, 1, CHUNCK_SIZE, file)) > 0) {
        fprintf(stdout, "Bloco %d:\n", i);
        printf("BYTES: %zd\n", bytesRead);
        printf("KILOBYTES: %zd\n", (bytesRead / 1024));
        printf("MEGABYTES: %zd\n\n", (bytesRead / (1024 * 1024)));
        send(socketDescriptor, buffer, bytesRead, 0);
        i++;
    }
    if (bytesRead <= 0) {
        fprintf(stdout, "ARQUIVO %s enviado\n", fileName);
    } else {
        fprintf(stdout, "ARQUIVO %s enviado com erro!\n", fileName);
    }
}

void sendNumbers(int socketDescriptor) {
    int index = 0;
    char * message[1024];

    printf("Estou enviando números!\n");
    while(1){
        sprintf(message, "%d", index);
        if (send(socketDescriptor, message, sizeof(message), 0) == -1) {
            perror("Erro ao enviar dados\n");
            exit(EXIT_FAILURE);
        }
        fprintf(stdout, "%d\n", index);
        index++;
        sleep(1);
    }
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

void configureSocket(struct sockaddr_in *socketAddr, int port);
int bindServerSocket(int serverSocketDescriptor, struct sockaddr_in *serverAddr, int port);
int awaitClient(int serverSocketDescriptor, struct sockaddr_in *clientAddr, socklen_t *clientAddrLen);
void listenConnections(int serverSocketDescriptor, int port, int backlog);


int bindServerSocket(int serverSocketDescriptor, struct sockaddr_in *serverAddr, int port){
    
    configureSocket(serverAddr, port);  

    if (bind(serverSocketDescriptor, (struct sockaddr *)serverAddr, sizeof(*serverAddr)) == -1) {
        fprintf(stderr,"Erro ao fazer bind\n");
        exit(EXIT_FAILURE);
    }

    return serverSocketDescriptor;
}


void configureSocket(struct sockaddr_in *socketAddr, int port){
    
    socketAddr->sin_family = AF_INET;                                    // Configura para que o socket trabalhe com endereços IPv4

    // Definindo a porta que o servidor vai escutar
    // A função htons serve para traduzir os bytes da porta para o formato correto
    socketAddr->sin_port = htons(port);
    socketAddr->sin_addr.s_addr = INADDR_ANY;                            // Definindo que o servidor receberá conexões de todos os endereços da máquina
}

void configureClientSocket(struct sockaddr_in *socketAddr, int port, char *server_ip){
    memset(socketAddr, 0, sizeof(socketAddr));

    // Configura para que o socket trabalhe com endereços IPv4
    socketAddr->sin_family = AF_INET;

    // Definindo a porta que o servidor vai escutar
    // A função htons serve para traduzir os bytes da porta para o formato correto
    socketAddr->sin_port = htons(port);

    if (inet_pton(AF_INET, server_ip, &socketAddr->sin_addr) <= 0) {
        fprintf(stderr, "Erro ao configurar o endereço do servidor\n");
        exit(EXIT_FAILURE);
    }
}

void listenConnections(int serverSocketDescriptor, int port, int backlog){

    fprintf(stdout, "Servidor ouvindo na porta %d\n", port);
    if (listen(serverSocketDescriptor, backlog) == -1) {
        fprintf(stderr, "Erro ao escutar\n");
        exit(EXIT_FAILURE);
    }
 
}

int awaitClient(int serverSocketDescriptor, struct sockaddr_in *clientAddr, socklen_t *clientAddrLen){
    int clientSocket = 0;
    fprintf(stdout, "Esperando a conexão...\n");

    // Aceita a conexão no socket do servidor
    // Sendo essa conexão no armazenada na memoria de clientAddr, e do tamanho da estrutura do mesmo
    clientSocket = accept(serverSocketDescriptor, (struct sockaddr *)clientAddr, clientAddrLen);
    if (clientSocket == -1) {
        fprintf(stderr,"Erro ao aceitar a conexão\n");
        exit(EXIT_FAILURE);
    }

    fprintf(stdout, "Conexão estabelecida com o Cliente %d!\n", clientSocket);

    return clientSocket;
}

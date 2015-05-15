
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>


#define PORT 3490 // puerto al que vamos a conectar 

#define MAXDATASIZE 100 // máximo número de bytes que se pueden leer de una vez 

#define MAX_RESPONSE_SIZE 1000


char show_menu(char op){
    printf("\n***************************\n");
    printf("    Escoja una opción\n\n");
    printf("0) Salir\n1) Listar salas\n2) Entrar en sala\n3) Crear sala\n4) Eliminar sala\n");
    printf("\nSu opción: ");
    scanf(" %c", &op);
    return op;
}

char * build_command(char op){
    /* Estructura del comando a construir:
        id_operacion#nombre-de-sala*/
    char *command = (char *)malloc(sizeof(char) * MAXDATASIZE);
    char *opt = (char *)malloc(sizeof(char) * 10);

    /*
    if ((op - 48) == 3){
        strcpy(opt, "1");
    }else{
        strcpy(opt, &op);
    }
    */
    strcpy(opt, &op);
    strcpy(command, opt);
    strcat(command, "#-");
    return command;
}

char * get_input(char * question){
    printf("%s: ", question);
    char *name = (char *)malloc(sizeof(char) * 30);
    scanf("%s", name);
    return name;
}

void process_request(int op){
    char *buf_response2 = (char *)malloc(sizeof(char) * MAX_RESPONSE_SIZE);
    switch(op){
        case 2:
            get_input("En qué sala desea entrar? (escriba el número)");
            break;
        case 3:
            printf("Crear nueva sala\n");
            get_input("Escriba el nombre de la nueva sala");

            // printf("archivo a eliminar: ----->'%s'\n", name2);

            // strcpy(command2, "3#");
            // strcat(command2, name2);


            // if (send(sockfd, command2, MAXDATASIZE-1, 0) == -1){
            //     perror("send");
            // }

            // if ((numbytes=recv(sockfd, buf_response2, MAX_RESPONSE_SIZE-1, 0)) == -1) {
            //     perror("recv");
            //     exit(1);
            // }

            // buf_response2[numbytes] = '\0';
            // printf("respuesta: '%s'\n", buf_response2);

            break;
        case 4:
            get_input("Qué sala desea eliminar? (escriba el número)");
            break;
        default:
            printf("[ERROR] no se especificó una operación válida\n");
            break;
    }
}


int main(int argc, char *argv[])
{
    int sockfd, numbytes;  
    char buf[MAXDATASIZE];
    struct hostent *he;
    struct sockaddr_in their_addr; // información de la dirección de destino 

    if (argc != 2) {
        fprintf(stderr,"usage: client hostname\n");
        exit(1);
    }

    if ((he=gethostbyname(argv[1])) == NULL) {  // obtener información de máquina 
        perror("gethostbyname");
        exit(1);
    }

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    memset(&(their_addr), 0, sizeof(their_addr));
    their_addr.sin_family = AF_INET;    // Ordenación de bytes de la máquina 
    their_addr.sin_port = htons(PORT);  // short, Ordenación de bytes de la red 
    their_addr.sin_addr = *((struct in_addr *)he->h_addr);
    //memset(&(their_addr.sin_zero), 8);  // poner a cero el resto de la estructura 

    if (connect(sockfd, (struct sockaddr *)&their_addr, sizeof(struct sockaddr)) == -1) {
        perror("connect");
        exit(1);
    }

    if ((numbytes=recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
        perror("recv");
        exit(1);
    }

    buf[numbytes] = '\0';

    printf("Conected: %s\n", buf);
    char *command = (char *)malloc(sizeof(char) * MAXDATASIZE);

    char op;
    char *buf_response = (char *)malloc(sizeof(char) * MAX_RESPONSE_SIZE);
    char *command2 = (char *)malloc(sizeof(char) * MAXDATASIZE);
    char *name2 = (char *)malloc(sizeof(char) * 30);
    while (1){
        printf("\n===================================\n");
        op = '\0';
        op = show_menu(op);
        command = "1#-"; // Siempre se trae la lista de salas
        //command = build_command(op);

        /*Se envía el comando para listar sala*/
        if (send(sockfd, command, MAXDATASIZE-1, 0) == -1){
            perror("send");
        }

        /*Se recibe la respuesta de la operación solicitada*/
        if ((numbytes=recv(sockfd, buf_response, MAX_RESPONSE_SIZE-1, 0)) == -1) {
            perror("recv");
            exit(1);
        }

        buf_response[numbytes] = '\0';

        /*Se analíza la operación solicitada para mostrar su resultado*/
        switch(op - 48){
            case 0:
                printf("Chao mundo\n");
                exit(0);
                break;
            case 1:
                printf("Lista de salas:\n");
                printf("%s\n", buf_response);
                break;
            default:
                printf("%s\n", buf_response);
                process_request(op -48);
                break;
        }
    }
    close(sockfd);

    return 0;
} 


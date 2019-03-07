/*
Tomas Mikna
*/

#include <winsock.h>
#define socklen_t int

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv [])
{
    WSADATA data;

    unsigned int port;
    int l_socket; //Server address structure
    int c_socket; // client socket structure

    struct sockaddr_in servaddr; // Serverio adreso strukt�ra
    struct sockaddr_in clientaddr; //client address struct
    // int clientaddrlen;
    socklen_t clientaddrlen;

    int s_len;
    int r_len;
    char buffer[1024];

    if (argc != 2)
    {
        printf ("USAGE: %s <port>\n", argv[0]);
        exit(1);
    }

    port = atoi(argv[1]);

    if (port < 1 || port > 65535)
    {
        printf("ERROR #1: invalid port specified.\n");
        exit(1);
    }

    WSAStartup(MAKEWORD(2,2), &data);              // function initiates use of the Winsock DLL by a process.

    //creating server socket
    if ((l_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        fprintf(stderr, "ERROR #2: can not create listening socket.\n");
        exit(1);
    }

    //I�valoma ir u�pildoma serverio adreso strukt�ra
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET; //choose protocol - IP

        /*
     * Nurodomas IP adresas, kuriuo bus laukiama klient�, �iuo atveju visi
     * esami sistemos IP adresai (visi interfeis'ai)
     */
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);    //htonl manage bytes order
    servaddr.sin_port = htons(port);  // nurodomas portas

    //Serverio adresas susiejamas su socket'u
    if (bind(l_socket, (struct sockaddr *)&servaddr, sizeof(servaddr))<0)
    {
        fprintf(stderr, "ERROR #3: bind listening socket.\n");
        exit(1);
    }


    /*
     * Nurodoma, kad socket'u l_socket bus laukiama klient� prisijungimo,
     * eil�je ne daugiau kaip 5 aptarnavimo laukiantys klientai
     */
    if (listen(l_socket, 5) != 0)
    {
        fprintf(stderr, "ERROR #4: error in listen().\n");
        exit(1);
    }

    for(;;)
    {
        //I�valomas buferis ir kliento adreso strukt�ra
        memset(&clientaddr, 0, sizeof(clientaddr));
        memset(&buffer, '\n', sizeof(buffer));

        //Waiting for connections
        clientaddrlen = sizeof(struct sockaddr);
        if ((c_socket = accept(l_socket, (struct sockaddr*)&clientaddr, &clientaddrlen)) < 0)
        {
            fprintf(stderr,"ERROR #5: error occured accepting connection.\n");
            exit(1);
        }

        //Reading client data when Client connects
        s_len = recv(c_socket, buffer, sizeof(buffer), 0);

        //Sendind data to client
        r_len = send(c_socket, buffer, s_len, 0);

        printf("IP: %s Sent: %d Received: %d\n", inet_ntoa(clientaddr.sin_addr), s_len, r_len);

        //Disconnect client
        close(c_socket);
    }

    return 0;
}


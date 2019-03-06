/*
Tomas Mikna
*/

#include <winsock.h>
#define socklen_t int

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFFLEN 1024
#define MAXCLIENTS 10

int findemptyuser(int c_sockets[])
{
    for (int i = 0; i < MAXCLIENTS; i++)
        if (c_sockets[i] == -1)
            return i;
    return -1;
}

int main(int argc, char *argv [])
{
    WSADATA data;

    unsigned int port;
    unsigned int clientaddrlen;
    int l_socket; //Server address structure
    int c_sockets[MAXCLIENTS]; // client socket structure
    fd_set read_set;

    struct sockaddr_in servaddr; // Serverio adreso struktûra
    struct sockaddr_in clientaddr; //client addrest struct
    // int clientaddrlen;
    //socklen_t clientaddrlen;

    int maxfd = 0;
    int i;

 //   int s_len;
 //   int r_len;
    char buffer[BUFFLEN];

    if (argc != 2)
    {
        fprintf (stderr, "USAGE: %s <port>\n", argv[0]);
        return -1;
        //exit(1);
    }

    port = atoi(argv[1]);

    if (port < 1 || port > 65535)
    {
        fprintf(stderr, "ERROR #1: invalid port specified.\n");
        return -1;
    }

    WSAStartup(MAKEWORD(2,2), &data);  // initiates use of the Winsock DLL by a process.
    //creating server socket    
    
    //Isvaloma ir uzpildoma serverio adreso struktûra
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET; //choose protocol - IP
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);    //htonl manage bytes order
    servaddr.sin_port = htons(port);  // nurodomas portas

    if ((l_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        fprintf(stderr, "ERROR #2: can not create listening socket.\n");
        return -1;
        //exit(1);
    }



        /*
     * Nurodomas IP adresas, kuriuo bus laukiama klientø, ðiuo atveju visi
     * esami sistemos IP adresai (visi interfeis'ai)
     */


    //Serverio adresas susiejamas su socket'u
    if (bind(l_socket, (struct sockaddr *)&servaddr, sizeof(servaddr))<0)
    {
        fprintf(stderr, "ERROR #3: bind listening socket.\n");
        return -1;
    }


    /*
     * Nurodoma, kad socket'u l_socket bus laukiama klientø prisijungimo,
     * eilëje ne daugiau kaip 5 aptarnavimo laukiantys klientai
     */
    if (listen(l_socket, 5) != 0)
    {
        fprintf(stderr, "ERROR #4: error in listen().\n");
        return -1;
    }

    for (i = 0; i < MAXCLIENTS; i++)
        c_sockets[i] = -1;

    for(;;)
    {
        FD_ZERO(&read_set);
        for(i = 0; i < MAXCLIENTS; i++)
            if (c_sockets[i] != -1)
            {
                FD_SET(c_sockets[i], &read_set);
                if (c_sockets[i] > maxfd)
                    maxfd = c_sockets[i];
            }

        FD_SET(l_socket, &read_set);
        if(l_socket > maxfd)
            maxfd = l_socket;

        printf("CHECKING: l_socket = %d\n",l_socket);

        select(maxfd + 1, &read_set, NULL, NULL, NULL);

        if (FD_ISSET(l_socket, &read_set))
        {
            int client_id = findemptyuser(c_sockets);
            if (client_id != -1)
            {
                clientaddrlen = sizeof(clientaddr);
                memset(&clientaddr, 0, clientaddrlen);
                c_sockets[client_id] = accept(l_socket, (struct sockaddr*)&clientaddr, &clientaddrlen);
                printf("Connected: %s\n", inet_ntoa(clientaddr.sin_addr));
            }
        }

        for (i = 0; i < MAXCLIENTS; i++)
            if (c_sockets[i] != -1)
                if (FD_ISSET(c_sockets[i], &read_set))
                {
                    memset(&buffer, 0, BUFFLEN);
                    int r_len = recv(c_sockets[i], buffer, BUFFLEN, 0);

                    for (int j = 0; j < MAXCLIENTS; j++)
                        if(c_sockets[j] != -1)
                        {
                            int w_len = send(c_sockets[j], buffer, r_len, 0);
                            printf("IP: %s Sent: %d Received: %d\n", inet_ntoa(clientaddr.sin_addr), w_len, r_len);
                            if (w_len <= 0)
                            {
                                close(c_sockets[j]);
                                c_sockets[j] = -1;
                            }
                        }
                }

/*        //Isvalomas buferis ir kliento adreso struktûra
        memset(&clientaddr, 0, sizeof(clientaddr));
        memset(&buffer, 0, sizeof(buffer));

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
*/
    }

    return 0;
}


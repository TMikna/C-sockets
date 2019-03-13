/*
Tomas Mikna
*/

#ifdef _WIN32
#include <winsock2.h>
#define socklen_t int
#else
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#define BUFFLEN 1024
#define MAXCLIENTS 10

char* receive_data(int c_sock, fd_set *master);
int accept_client(int l_socket, fd_set *master);
void error_check();

int main(int argc, char *argv [])
{
    WSADATA data;

    char *buffer = malloc(sizeof(char)*BUFFLEN);
    fd_set master;    //master file descriptor list
    fd_set read_set;  //temp file descriptor list for select(), since select change set and we need to keep track of all connected sockets
    int maxfd = 0;    // maximum file descriptor number
    int l_socket;     // listening socket
    int port;
    struct sockaddr_in servaddr; // Serverio adreso struktûra



    if (argc != 2)
    {
        fprintf (stderr, "USAGE: %s <port>\n", argv[0]);
        return -1;
        //exit(1);
    }

    FD_ZERO(&master);  //clean both sets
    FD_ZERO(&read_set);

    port = atoi(argv[1]);


    if (port < 1 || port > 65535)
    {
        fprintf(stderr, "ERROR #1: invalid port specified.\n");
        return -1;
    }

#ifdef _WIN32
    int res = WSAStartup(MAKEWORD(2,2), &data);  // initiates use of the Winsock DLL by a process.
    if(res != 0)
		fprintf(stderr, "ERROR: WSAStarup failed");
#endif



    if ((l_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        fprintf(stderr, "ERROR #2: can not create listening socket.\n");
        return -1;
    }
    //Isvaloma ir uzpildoma serverio adreso struktûra
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET; //choose protocol - IP
    servaddr.sin_addr.s_addr = INADDR_ANY;    //htonl manage bytes order
    servaddr.sin_port = htons(port);  // nurodomas portas

        // lose the pesky "address already in use" error message
    int yes=1;

    if (setsockopt(l_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&yes, sizeof(int)) < 0)
    {
        fprintf(stderr, "ERROR #3: setsockopt() error.\n");
        return -1;
    }

    //Serverio adresas susiejamas su socket'u
    if (bind(l_socket, (struct sockaddr *)&servaddr, sizeof(servaddr))<0)
    {
        fprintf(stderr, "ERROR #4: bind listening socket.\n");
        return -1;
    }


    /*
     * Nurodoma, kad socket'u l_socket bus laukiama klientø prisijungimo,
     * eileje ne daugiau kaip 5 aptarnavimo laukiantys klientai
     */
    if (listen(l_socket, 5) != 0)
    {
        fprintf(stderr, "ERROR #5: error in listen().\n");
        return -1;
    }

    FD_SET(l_socket, &master);      //add the listener to the master set
    maxfd = l_socket;               //keep track of the biggest file descriptor




    for(;;)
    {

        read_set = master; // copy it
        printf("read_set eelemts: %d\n", read_set.fd_count);
        printf("maxfd: %d \n", maxfd);
        if (select (maxfd + 1, &read_set, NULL, NULL, NULL) < 0)
        {
            perror("ERROR #6: select()");
            return -1;
        }
        for (int i = 0; i <= maxfd; i++)
        {
//printf ("kažkas: %d          ", i);
            if (FD_ISSET(i, &read_set))   //something is set
            {
                printf ("kažkas is set\n");
                if (i == l_socket)        //someone wants to connect
                {
                    int newfd = accept_client(l_socket, &master);
                    if (newfd > maxfd)
                        maxfd = newfd;
                }
                else
                {
                    int rec_len = 0;
                    buffer = receive_data(i, &master);
                    //memset (buffer, '\0', sizeof(char)*BUFFLEN);
                    //recv(i, buffer, sizeof(buffer), 0);
                    printf("receive_data grazino: %s\n", buffer);
                        for (int j = 0; j <= maxfd; j++)     //send for everyone
                            if (FD_ISSET(j, &master))
                                if (j != l_socket && j != i) // except sender and listening socket
                                    if(send (j, buffer, rec_len, 0) == -1)
                                        perror("send");

                }

            }

        }
        printf ("FOREND\n");
    }

    WSACleanup();
    return 0;
}


int accept_client(int l_socket, fd_set *master)
{
    int new_fd;
    char ip_address[16];
    struct sockaddr c_address;
    memset (&c_address, 0, sizeof(c_address));
    socklen_t size = sizeof(c_address);
    new_fd = accept (l_socket, &c_address, &size);
    if (new_fd <= -1)
    {
        perror("accept");
        return -1;
    }
    printf("Do 1");

    FD_SET(new_fd, master);
    printf("Do 2");

    printf("New connection on socket %d.\n", new_fd);


    return new_fd;
}


char* receive_data(int c_sock, fd_set *master)
{
    char *buffer = malloc(sizeof(char)*BUFFLEN);
    int len = 0;
    int resp = 0;

    memset (buffer, '\0', sizeof(char)*BUFFLEN);

    
    if(len = recv(c_sock, buffer+resp, sizeof(buffer), 0) <= 0)
    {

        close(c_sock);
        FD_CLR(c_sock, master);
        if (len == 0)
        {
            printf("Socket %d was closed by client\n", c_sock);
        }
        else
        {
            perror("recv");
        }
    }

    printf("Message: %s", buffer);
    return buffer;
}

void error_check()
{
            wchar_t *s = NULL;
        FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
               NULL, WSAGetLastError(),
               MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
               (LPWSTR)&s, 0, NULL);
        fprintf(stderr, "%S\n", s);
}

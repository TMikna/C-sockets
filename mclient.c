/*
Tomas Mikna
*/

#include <winsock2.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <fcntl.h>

#define BUFFLEN 1024

int main(int argc, char *argv[])
{
WSADATA data;

unsigned int port;
int s_socket;
struct sockaddr_in servaddr; //server address structure
fd_set read_set;

char recvbuffer[BUFFLEN];
char sendbuffer[BUFFLEN];

int length;

if (argc != 3)
{
    fprintf(stderr, "USAGE: %s <ip> <port>\n", argv[0]);
    exit(1);
}

port = atoi(argv[2]);  //converts string into int

if ((port < 1) || (port > 65535))
{
    printf("ERROR #1: invalid port specified.\n");
    exit(1);
}

WSAStartup(MAKEWORD(2,2), &data);

//creating socket
if ((s_socket = socket(AF_INET, SOCK_STREAM,0)) < 0)
{
    fprintf(stderr, "ERROR #2: cannot create socket.\n");
    exit(1);
}

//Isvaloma ir uzpildoma serverio struktûra
memset(&servaddr, 0, sizeof(servaddr));
servaddr.sin_family = AF_INET; // nurodomas protokas - IP
servaddr.sin_port = htons(port); // nurodomas port'as, htons reikia kad bûtø uþtinrinta baitø saugojimo tvarka

    /*
     * Isverciamas simboliu eilutëje uzrasytas ip i skaitine forma ir
     * nustatomas serverio adreso struktûroje.
     */
//#ifdef _WIN32
    servaddr.sin_addr.s_addr = inet_addr(argv[1]);
//#else
//    if (inet_aton(argv[1], &servaddr.sin_addr) <= 0)
//    {
//        fprintf (stderr, "ERROR #3: Invalid remote IP address.\n");
//        exit(1);
//    }
//#endif

// connect to the server
if (connect(s_socket, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0)
{
    fprintf(stderr, "ERROR #4: error in connect().\n");
    exit(1);
}

memset(&sendbuffer, 0, BUFFLEN);
sendbuffer[0] = '\n';
int res = write(s_socket, sendbuffer, sizeof(sendbuffer));
memset(&sendbuffer, 0, BUFFLEN);

//fcntl(0, F_SETFL, fcntl(0, F_GETFL, 0) | O_NONBLOCK);   // works with file descriptors

//for (;;)

/*    FD_ZERO(&read_set);
    FD_SET(s_socket, &read_set);
    FD_SET(0, &read_set);

    select(s_socket + 1, &read_set, NULL, NULL, NULL);

    if (FD_ISSET(s_socket, &read_set))
    {
        memset(&recvbuffer, 0, BUFFLEN);
        i = recv(s_socket, recvbuffer, BUFFLEN, 0);
        printf("%s nonono\n", recvbuffer);
    }
    if (FD_ISSET(0, &read_set))
    {
        //i = read(0, &sendbuffer, 1);
        fgets(sendbuffer, BUFFLEN, stdin);
        send(s_socket, sendbuffer, strlen(sendbuffer), 0);
    }
*/
    printf("Write your message or press enter \n");

    for (;;)
    {
        int n = 0;
        printf ("\n");

    //    while ((sendbuffer[n++] = getchar()) != '\n');
    //    if (n>0){
        fgets(sendbuffer, BUFFLEN, stdin);
        printf("you wrote : %s\n", sendbuffer);


        int res = write(s_socket, sendbuffer, sizeof(sendbuffer));
        if(res <= 0)
		{
		    wchar_t *s = NULL;
        FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
               NULL, WSAGetLastError(),
               MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
               (LPWSTR)&s, 0, NULL);
        fprintf(stderr, "%S\n", s);
			//0==other side terminated conn
			printf("res =  %d.\n", res);
			printf("\nSocket error happened or SERVER terminated connection\n");
			close(s_socket);
			s_socket = 0;
			break;
		}
        memset(sendbuffer, 0, BUFFLEN);


        if ((length = read(s_socket, recvbuffer, sizeof(recvbuffer))) > 0)
            printf("Server: \n%s\n", recvbuffer);
        else
        {
            printf("Socket error happened or SERVER terminated connection 2.\n");
            close(s_socket);
			s_socket = 0;
            break;
        }

        if ((strncmp(recvbuffer, "exit", 4)) == 0)
        {
            printf("Client Exit...\n");
            break;
        }

        memset(recvbuffer, 0, sizeof(recvbuffer));
    }

//printf ("Enter the message: ");
//fgets(buffer, BUFFLEN, stdin);

//send message to server
//send(s_socket, buffer, strlen(buffer), 0);

//memset(&buffer, 0, BUFFLEN);

//get message from server
//recv(s_socket, buffer, BUFFLEN, 0);
//printf ("Server sent: %s\n", buffer);

//close the socket
close(s_socket);
return 0;
}











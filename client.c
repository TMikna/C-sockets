/*
Tomas Mikna
*/

#include <winsock2.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFFLEN 1024

int main(int argc, char *argv[])
{
WSADATA data;

unsigned int port;
int s_socket;
struct sockaddr_in servaddr; //server address structure

char buffer[BUFFLEN];

if (argc != 3)
{
    fprintf(stderr, "USAGE: %s <ip> <port>\n", argv[0]);
    exit(1);
}

port = atoi(argv[2]);

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

//Iðvaloma ir uþpildoma serverio struktûra
memset(&servaddr, 0, sizeof(servaddr));
servaddr.sin_family = AF_INET; // nurodomas protokas - IP
servaddr.sin_port = htons(port); // nurodomas port'as, htons reikia kad bûtø uþtinrinta baitø saugojimo tvarka

    /*
     * Iðverèiamas simboliø eilutëje uþraðytas ip á skaitinæ formà ir
     * nustatomas serverio adreso struktûroje.
     */

servaddr.sin_addr.s_addr = inet_addr(argv[1]);


// connect to the server

if (connect(s_socket, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0)
{
    fprintf(stderr, "ERROR #4: error in connect().\n");
    exit(1);
}

printf ("Enter the message: ");
fgets(buffer, BUFFLEN, stdin);

//send message to server
send(s_socket, buffer, sizeof(buffer), 0);
printf("Sent: : %s", buffer);
memset(&buffer, 0, BUFFLEN);

//get message from server
recv(s_socket, buffer, BUFFLEN, 0);
printf ("Server sent: %s\n", buffer);

//close the socket
close(s_socket);
WSACleanup();
return 0;
}











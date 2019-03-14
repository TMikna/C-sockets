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

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <fcntl.h>

#define BUFFLEN 1024


void *my_send (void *s_socket);
void *my_receive (void *s_socket);


int main(int argc, char *argv[])
{
WSADATA data;

unsigned int port;
int s_socket;
struct sockaddr_in servaddr; //server address structure

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

printf ("Connected to server\n");
//memset(&sendbuffer, '\n', BUFFLEN);
//int res = write(s_socket, sendbuffer, sizeof(sendbuffer));
//memset(&sendbuffer, 0, BUFFLEN);

//unsigned long b=1;
//ioctlsocket(s_socket,FIONBIO,&b);
//fcntl(0, F_SETFL, fcntl(0, F_GETFL, 0) | O_NONBLOCK);   // non blocking mode, linux

  pthread_t send_thread, receive_thread;
  void *ret;
  int *sock_ptr = malloc(sizeof(*sock_ptr));
  *sock_ptr = s_socket;

  if (pthread_create(&send_thread, NULL, my_send, (void*)&s_socket) != 0){
    perror("pthread_create(send_thread)  error");
    exit(1);
  }

    if (pthread_create(&receive_thread, NULL, my_receive, (void*)&s_socket) != 0){
        perror("pthread_create(receive_thread)  error");
        exit(1);
  }

    if (pthread_join(send_thread, NULL) != 0) {
        perror("pthread_join(send_thread) error");
        exit(3);
  }

    if (pthread_join(receive_thread, NULL) != 0) {
        perror("pthread_join(receive_thread) error");
        exit(3);
  }


/*
    printf("Write your message or press enter \n");

    for (;;)
    {
        int n = 0;
        printf ("\n");

        while ((sendbuffer[n++] = getchar()) != '\n');
        if (n>0)
        {
    //    fgets(sendbuffer, BUFFLEN, stdin);
        printf("you wrote : %s\n", sendbuffer);

    //    if (sendbuffer[0] != '\n')
    //    {
            int res = 0;
            while (res != sizeof(sendbuffer) && res != -1)
                res += write(s_socket, sendbuffer, sizeof(sendbuffer));
            int errCode = WSAGetLastError();
            printf("res =  %d.\n", res);
            if(res <= 0)
            {


        fprintf (stderr, "WSAGetLastError errCode = %d\n", errCode);

                //0==other side terminated conn
                printf("res =  %d.\n", res);
                printf("\nSocket error happened or SERVER terminated connection\n");
                close(s_socket);
                s_socket = 0;
                break;
            }

            memset(sendbuffer, '\n', BUFFLEN);
        }

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
*/
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
WSACleanup();
return 0;
}

void *my_send (void *s_socket)
{
        int socket = *((int *)s_socket);
        char sendbuffer[BUFFLEN];
        char buffer[BUFFLEN-4];
        int total, len;

        for(;;)
        {
            memset(sendbuffer, '\0', sizeof(char)*BUFFLEN);
            len = 0;
            //fgets(sendbuffer, BUFFLEN, stdin);
            printf("%s","> ");
            scanf("%[^\n]%*c", buffer);
            if (buffer[0] != '\n' || buffer[0] != '\0')
            {
                total = strlen(buffer)+1;
                sendbuffer[0] = total;
                strcpy(sendbuffer+1, buffer);

                while (len < total)
                {
                    int sent = 0;
                    sent = send(socket, sendbuffer+len, strlen(sendbuffer)-len, 0);
                    if (sent < 0)
                        perror("send");
                    len += sent;
                    printf("sent: %d, len:%d, total: %d\n",sent, len, total);
                }
            }
        }
}

void *my_receive (void *s_socket)
{
    int socket = *((int *)s_socket);
    char recvbuffer[BUFFLEN];
    int i;

    fd_set read_set;
    FD_ZERO(&read_set);
    FD_SET(socket, &read_set);
    for (;;)
    {


        select(socket + 1, &read_set, NULL, NULL, NULL);
        if (FD_ISSET(socket, &read_set))
        {
            memset(&recvbuffer, '\0', BUFFLEN);
            if(i = recv(socket, recvbuffer, sizeof(char)*BUFFLEN-2, 0) <= 0)
            {
                close(socket);
                if (i == 0)
                {
                    printf("Socket %d was closed by client\n", socket);
                    exit(0);
                }
                else
                {
                    perror("recv");
                    exit(1);

                }
            }
            printf("Server: %s\n", recvbuffer);
        }

    }
}











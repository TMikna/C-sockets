/*
Tomas Mikna
*/

#include <winsock.h>
#define socklen_t int

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#define BUFFLEN 1024
#define MAXCLIENTS 10

struct Client
{
    bool connected;    // true if client is connected
    struct sockaddr_in addr;  // client info
    int c_socket;      // client socket
    fd_set fdset;      // used to check if there is data in the socket
    int i;             // any additional info
};

int findemptyuser(int c_sockets[])
{
    for (int i = 0; i < MAXCLIENTS; i++)
        if (c_sockets[i] == -1)
            return i;
    return -1;
};

int my_accept (struct Client *cli);
int my_send (struct Client *cli, char *buffer,int sz);
int my_recv(struct Client *cli, char *buffer, int sz);
void accept_clients();
void disconnect(struct Client *cli); //this is called by the low level funtions
void chat_message(char *s);          // send a message to all chat participants
void recv_client();


int l_socket; //Server address structure
struct Client client[MAXCLIENTS]; // client socket structure

int main(int argc, char *argv [])
{
    WSADATA data;

    unsigned int port;
    unsigned int clientaddrlen;

 //   int c_sockets[MAXCLIENTS]; // client socket structure //unused
 //   fd_set read_set;

    struct sockaddr_in servaddr; // Serverio adreso struktûra
 //   struct sockaddr_in clientaddr; //client addrest struct
    // int clientaddrlen;
    //socklen_t clientaddrlen;

 //   int maxfd = 0;
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

    int res = WSAStartup(MAKEWORD(2,2), &data);  // initiates use of the Winsock DLL by a process.
    if(res != 0)
		fprintf(stderr, "ERROR: WSAStarup failed");


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

    unsigned long b=1;
	ioctlsocket(l_socket,FIONBIO,&b); // set non-blocking mode (b=1), that means functions return even if they got no response


 //   for (i = 0; i < MAXCLIENTS; i++)
 //       c_sockets[i] = -1;

    for(;;)
    {
        accept_clients();
        recv_client();

/*        FD_ZERO(&read_set);
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
*/
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

    WSACleanup();
    return 0;
}

int my_accept (struct Client *cli)
    {
        struct sockaddr x;
        cli -> i = sizeof(struct sockaddr);
        if((cli -> c_socket = accept(l_socket, (struct sockaddr*)&cli -> addr, &cli->i)) >= 0)
        {
            cli->connected = true;
            FD_ZERO(&cli->fdset);
            FD_SET(cli->c_socket, &cli->fdset);

            unsigned long b=1;
            ioctlsocket(cli -> c_socket,FIONBIO,&b);

            return true;
        }
        return false;
    }

int my_send(struct Client *cli, char *buffer, int sz)
{
    if (send(cli->c_socket,buffer,sz,0) > 0);
        return true;
    disconnect(cli);
    return false;
}

int my_recv(struct Client *cli, char *buffer, int sz)
{

    int res;
    select(cli -> c_socket + 1, &cli->fdset, NULL, NULL, NULL);
	if(FD_ISSET(cli->c_socket,&cli->fdset))
	{
        res = recv(cli->c_socket, buffer, sizeof(buffer), 0);
        int errCode = WSAGetLastError();

        fprintf (stderr, "WSAGetLastError errCode = %d\n", errCode);


//        wchar_t *s = NULL;
//        FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
//               NULL, WSAGetLastError(),
//               MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
//               (LPWSTR)&s, 0, NULL);
//        fprintf(stderr, "%S\n", s);

        if (buffer[0] != '\n')
            printf("OPasiekė toks:  %s.\n", buffer);
        printf ("ERROR 2, res = %d\n", res);
		if (res != 0)
            return true;
        else
            disconnect(cli);
    }
	printf ("ERROR 1, res = %d\n", res);
	return false;
}

void accept_clients()
{
	for(int i=0; i<MAXCLIENTS; i++)
	{
		if(!client[i].connected)		//i.e a client has not connected to this slot
		{
			if(my_accept(&client[i]))  // ar tikrai reikia &?
			{
                printf("Client connected!\n");
			}
		}
	}
}

void disconnect(struct Client *cli) //this is called by the low level funtions
{
	if(cli->c_socket)
        closesocket(cli->c_socket);
	cli->connected = false;
	cli->i = -1;
	printf("Client disconnected.\n");
}

void chat_message(char *s)          // send a message to all chat participants
{
	int len = strlen(s);

	for(int i=0; i<MAXCLIENTS; i++)
	{
		if(client[i].connected)		//valid slot,i.e a client has parked here
		{
			my_send(&client[i],s,len);
		}
	}
}

void recv_client()
{
    char buffer[BUFFLEN];

	for(int i = 0; i<MAXCLIENTS; i++)
	{
		if(client[i].connected)		//valid slot,i.e a client has parked here
		{
            memset(&buffer, 0, BUFFLEN);
		    buffer[0] = '\n';
			if(my_recv(&client[i], buffer, BUFFLEN))
			{
			    //printf("OPasiekė:  %s.\n", bufferr);
				if(buffer[0]=='/')
				{
					//respond to commands
					if(strcmp(buffer,"/server_bang")==0)
					{
					    //printf("Received: %s", buffer);
						chat_message("8*8* The Server Goes BANG *8*8");
					}
				}
				else if (buffer[0] != '\n')
				{
				    //printf("Received 01: %s", buffer);
					chat_message(buffer);
				}
			}
		}
	}
}
/*void error_check()
{
            wchar_t *s = NULL;
        FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
               NULL, WSAGetLastError(),
               MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
               (LPWSTR)&s, 0, NULL);
        fprintf(stderr, "%S\n", s);
}*/

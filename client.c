#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <ctype.h>
#include <arpa/inet.h>

int main(int argc , char *argv[])
{
    int sockfd = -1;
    struct sockaddr_in server_addr;
    char valread[1024];
    char valsend[1024];
    int stop = 1;

    memset(valsend, 0 , sizeof(valsend)); // khoi tao vung nho
    memset(valread, 0 , sizeof(valread)); 
	memset(&server_addr, 0 , sizeof(server_addr));

    sockfd = socket(AF_INET, SOCK_STREAM, 0); // khoi tao socket
	server_addr.sin_family = AF_INET; // thong tin server
	server_addr.sin_addr.s_addr = inet_addr("192.168.1.217");
	server_addr.sin_port = htons(5000);

    if(connect(sockfd, (struct  sockaddr *)&server_addr, sizeof(server_addr))<0) // connect toi sever
    {
        printf("Connection err\n");
        exit(EXIT_FAILURE);
    }

    while (stop!=0)
    {          
        printf("Client: ");
        fgets(valsend, sizeof(valsend), stdin); // nhap thong tin can gui
        fflush(stdin);
        send(sockfd, valsend, sizeof(valsend), 0);  //gui thong tin den server
        stop = strcmp(valsend, "x\n"); // so sanh
    }
    close(sockfd); 
    exit(EXIT_SUCCESS);
}
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <pthread.h>

struct sockaddr_in server_addr;
int listenfd = -1; 
char valinput[1024];
pthread_t thread[3], exit_main;

typedef struct mysocket{
    struct sockaddr_in client_addr;
    char addr[100];
    int stop;
    int port;
    int addr_len;
    int new_socket_len;
    int new_socket;     
    char valsend[1024];
    char valread[1024];
} mysocket;

mysocket data_thread[3];

void *input()
{
    while(strcmp("x\n", valinput)!=0)
    {
        fgets(valinput, sizeof(valinput), stdin);
        fflush(stdin);
    }
    pthread_cancel(thread[0]);
    pthread_cancel(thread[1]);
    pthread_cancel(thread[2]);
}

void *mythread0()
{
    data_thread[0].new_socket = accept(listenfd, (struct sockaddr*)&data_thread[0].client_addr, (socklen_t*)&data_thread[0].addr_len); // accept ket noi cua client va tao ra socket moi la new_socket, dong thoi luu thong tin cua client vao client_addr
    if (data_thread[0].new_socket<0)
    {
        printf("Accept failed\n"); // thoat neu loi
        exit(EXIT_FAILURE);
    }

    strcpy(data_thread[0].addr ,inet_ntoa(data_thread[0].client_addr.sin_addr)); // copy dia chi IP cua client vao addr
    data_thread[0].port = ntohs(data_thread[0].client_addr.sin_port); // gan port cua client vao port
    printf("%s:%d connected\n", data_thread[0].addr, data_thread[0].port);

    while (1)
    {        
        recv(data_thread[0].new_socket, data_thread[0].valread, sizeof(data_thread[3].valread), 0); // cho nhan  thong tin tu client
        data_thread[0].stop = strcmp(data_thread[0].valread, "x\n");    //neu gia tri nhan duoc la x thi thoat  
        if ( data_thread[0].stop == 0) break;
        printf("Client (%s:%d): %s", data_thread[0].addr, data_thread[0].port, data_thread[0].valread); // in ra thong tin cua client
    }

    printf("%s:%d disconnected\n", data_thread[0].addr, data_thread[0].port); 
    close(data_thread[0].new_socket); // dong socket

    pthread_join(thread[0], NULL); //huy thread
    pthread_create(&thread[0],NULL, mythread0, NULL);//tao lai thread
}

void *mythread1()
{
    data_thread[1].new_socket = accept(listenfd, (struct sockaddr*)&data_thread[1].client_addr, (socklen_t*)&data_thread[1].addr_len); 
    if (data_thread[1].new_socket<0)
    {
        printf("Accept failed\n");
        exit(EXIT_FAILURE);
    }   
    strcpy(data_thread[1].addr ,inet_ntoa(data_thread[1].client_addr.sin_addr)); 
    data_thread[1].port = ntohs(data_thread[1].client_addr.sin_port); 
    printf("%s:%d connected\n", data_thread[1].addr, data_thread[1].port);

    while (1)
    {        
        recv(data_thread[1].new_socket, data_thread[1].valread, sizeof(data_thread[1].valread), 0);
        data_thread[1].stop = strcmp(data_thread[1].valread, "x\n");
        if ( data_thread[1].stop == 0) break;
        printf("Client (%s:%d): %s", data_thread[1].addr, data_thread[1].port, data_thread[1].valread);    
    }
    printf("%s:%d disconnected\n", data_thread[1].addr, data_thread[1].port); 
    close(data_thread[1].new_socket);

    pthread_join(thread[1], NULL);
    pthread_create(&thread[1],NULL, mythread1, NULL);
}

void *mythread2()
{
    data_thread[2].new_socket = accept(listenfd, (struct sockaddr*)&data_thread[2].client_addr, (socklen_t*)&data_thread[2].addr_len);
    if (data_thread[2].new_socket<0)
    {
        printf("Accept failed\n");
        exit(EXIT_FAILURE);
    }   
    strcpy(data_thread[2].addr ,inet_ntoa(data_thread[2].client_addr.sin_addr));
    data_thread[2].port = ntohs(data_thread[2].client_addr.sin_port);
    printf("%s:%d connected\n", data_thread[2].addr, data_thread[2].port);

    while (1)
    {        
        recv(data_thread[2].new_socket, data_thread[2].valread, sizeof(data_thread[2].valread), 0);
        data_thread[2].stop = strcmp(data_thread[2].valread, "x\n");
        if ( data_thread[2].stop == 0) break;
        printf("Client (%s:%d): %s", data_thread[2].addr, data_thread[2].port, data_thread[2].valread);
    } 
    printf("%s:%d disconnected\n", data_thread[2].addr, data_thread[2].port); 
    close(data_thread[2].new_socket);

    pthread_join(thread[2], NULL);
    pthread_create(&thread[2],NULL, mythread2, NULL);
}

int main(int argc , char *argv[])
{
    for(int i = 0; i<3; i++)
    {
        data_thread[i].new_socket = -1;
        data_thread[i].stop = 1;
        data_thread[i].addr_len = sizeof(data_thread[i].addr);
        data_thread[i].new_socket_len = sizeof(data_thread[i].new_socket);
        memset(data_thread[i].valsend, 0 , sizeof(data_thread[i].valsend));
        memset(data_thread[i].valread, 0 , sizeof(data_thread[i].valread));
        memset(&data_thread[i].client_addr, 0 , sizeof(data_thread[i].client_addr));
    }
    memset(&server_addr, 0 , sizeof(server_addr));

    listenfd = socket(AF_INET, SOCK_STREAM, 0); // tao socket

    if(listenfd<0)
    {
        printf("Socket failed\n"); //thoa neu loi
        exit(EXIT_FAILURE);
    }
    server_addr.sin_family = AF_INET;  // truong family
    server_addr.sin_addr.s_addr = inet_addr("192.168.1.217"); // dia chi server
    server_addr.sin_port = htons(5000); // port server

    if(bind(listenfd, (struct sockaddr*)&server_addr, sizeof(server_addr))<0) // dinh kem dia chi va port sever vao socket
    {
        printf("Bind failed\n"); // thoat neu loi
        exit(EXIT_FAILURE);
    }
    listen(listenfd, 10); // lang nghe, hang doi toi da la 10

    printf("Waiting for client: \n");
    
    pthread_create(&thread[0], NULL, mythread0, NULL);
    pthread_create(&thread[1], NULL, mythread1, NULL);
    pthread_create(&thread[2], NULL, mythread2, NULL);
    pthread_create(&exit_main, NULL, input, NULL);   //nhap x de ket thuc server
    
    pthread_join(thread[0], NULL);
    pthread_join(thread[1], NULL);
    pthread_join(thread[2], NULL);
    pthread_join(exit_main, NULL);

    printf("The server end!\n");
    close(listenfd);
    exit(EXIT_SUCCESS);
}

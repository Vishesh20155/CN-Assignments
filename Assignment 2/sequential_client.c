#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>


int main(int argc, char const* argv[])
{
    int sockfd, clientfd, portno=8080, n;
    socklen_t clilen;
    char buffer[256];
    struct sockaddr_in serv_addr;

    sockfd=socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd<0)
    {
        perror("Error in using socket() syscall");
        exit(EXIT_FAILURE);
    }

    bzero((char*)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_port=htons(portno);

    int convertip = inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);
    if(convertip<0){
        perror("Error in inet_pton()");
        exit(EXIT_FAILURE);
    }

    clientfd=connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    if (clientfd<0)
    {
        perror("Error in connect()");
        exit(EXIT_FAILURE);
    }
    
    // write(sockfd, "Hi from Client", 255);

    int i=1;

    while (i<=20)
    {
        char buf[256];
        sprintf(buf, "%d", i);
        int wr=write(sockfd, buf, 255);
        if (wr<0)
        {
            perror("Error in write() syscall");
            exit(EXIT_FAILURE);
        }

        bzero(buffer, 256);
        n=read(sockfd, buffer, 255);
        if (n<0)
        {
            perror("Error in using read() syscall");
            exit(EXIT_FAILURE);
        }

        printf("Response from server to client: %d! = %s\n\n",i, buffer);

        i++;
    }

    int wr=write(sockfd, "end", 255);
    if (wr<0)
    {
        perror("Error in write() syscall");
        exit(EXIT_FAILURE);
    }
    

    printf("Written to server by client.... Waiting for response.....\n\n");
    
    bzero(buffer, 256);
    n=read(sockfd, buffer, 255);
    if (n<0)
    {
        perror("Error in using read() syscall");
        exit(EXIT_FAILURE);
    }

    printf("%s\n\n", buffer);

    close(clientfd);
    return 0;
}
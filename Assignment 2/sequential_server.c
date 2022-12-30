#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <fcntl.h> 
#include <arpa/inet.h>


long long factorial(int x){
    long long ans=1;
    int i=2;
    while (i<=x)
    {
        ans=ans*i;
        i++;
    }
    
    return ans;    
}

int main(int argc, char const *argv[])
{
    int sockfd, newsockfd, portno=8080, n;
    socklen_t clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    char newline_char='\n', colon_char=':', space_char=' ';


    // Creating a socket using socket() syscall
    sockfd=socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd<0)
    {
        perror("Error in using socket() syscall");
        exit(EXIT_FAILURE);
    }
    
    bzero((char*)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_addr.s_addr=INADDR_ANY;
    serv_addr.sin_port=htons(portno);

    
    // Binding the socket:
    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))<0)
    {
        perror("Error in using bind() syscall");
        exit(EXIT_FAILURE);
    }

    listen(sockfd, 11);
    clilen=sizeof(cli_addr);

    newsockfd=accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
    if (newsockfd < 0) 
    {
        perror("Error in using accept() syscall");
        exit(EXIT_FAILURE);
    }
    printf("Connected\n");
    // char* ip_address;
    // inet_ntop(AF_INET, &(cli_addr.sin_addr.s_addr), ip_address, INET_ADDRSTRLEN);
    // printf("%s\n", ip_address);
    // printf("\n\n");

    int fd = open("sequential_server_file.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd<0)
    {
        perror("Error in open() syscall");
        exit(EXIT_FAILURE);
    }



    while (1)
    {
        bzero(buffer, 256);

        n=read(newsockfd, buffer, 255);
        if (n<0)
        {
            perror("Error in using read() syscall");
            exit(EXIT_FAILURE);
        }

        if (strcmp(buffer, "end")==0)
        {
            break;
        }
        int x=atoi(buffer);
        printf("Received at server: %d\n", x);
        long long y=factorial(x);
        char char_y[20];
        sprintf(char_y, "%lld", y);
        printf("Computed at server: %s\n", char_y);

        int wr=write(newsockfd, char_y, 255);
        if (wr<0)
        {
            perror("Error in write()");
            exit(EXIT_FAILURE);
        }

        if(x>0){
            char* char_y1 = char_y;
            strncat(char_y1, &newline_char, 1);

            char* char_y2=inet_ntoa(cli_addr.sin_addr);
            strncat(char_y2, &colon_char, 1);
            int port_x=ntohs(cli_addr.sin_port);
            char char_port[256];
            sprintf(char_port, "%d", port_x);
            strncat(char_y2, char_port, strlen(char_port));
            strncat(char_y2, &space_char, 1);
            strncat(char_y2, char_y1, strlen(char_y1));

            int wr_file=write(fd, char_y2, strlen(char_y2));
            if (wr_file<0)
            {
                perror("Error in write() for file");
                exit(EXIT_FAILURE);
            }
        }

    }

    printf("\n");
    int wr=write(newsockfd, "Exit from Server", 255);
    if (wr<0)
    {
        perror("Error in write()");
        exit(EXIT_FAILURE);
    }
    

    close(newsockfd);
    close(fd);

    // shutdown(sockfd, SHUT_RDWR);
    return 0;
}

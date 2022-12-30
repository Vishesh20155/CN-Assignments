// Use sequential client for testing

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
#include <pthread.h>

pthread_mutex_t lock;

struct fd_info
{
    int fd, file_fd;
    struct sockaddr_in addr;
};

char newline_char='\n', colon_char=':', space_char=' ';

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

void *handleAccept(void *varg){
    pthread_mutex_lock(&lock);
    struct sockaddr_in cli_addr=((struct fd_info*)varg)->addr;
    int newsockfd=((struct fd_info*)varg)->fd;
    int fd=((struct fd_info*)varg)->file_fd;
    char buffer[256];
    printf("\nAccepted connection from %s:%d\n", inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));
    while (1)
    {
        bzero(buffer, 256);

        int n=read(newsockfd, buffer, 255);
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
    // int wr=write(newsockfd, "Exit from Server", 255);
    // if (wr<0)
    // {
    //     perror("Error in write()");
    //     exit(EXIT_FAILURE);
    // }
    

    close(newsockfd);
    pthread_mutex_unlock(&lock);
    return NULL;
}

int main()
{
    int val;
	int i=1;

    int sockfd, newsockfd, portno=8080;
    socklen_t clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    char newline_char='\n', colon_char=':', space_char=' ';

    int mut_ret = pthread_mutex_init(&lock, NULL);

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

    listen(sockfd, 25);
    clilen=sizeof(cli_addr);

    int fd = open("thread_server_file.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd<0)
    {
        perror("Error in open() syscall");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        newsockfd=accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
        if (newsockfd < 0) 
        {
            perror("Error in using accept() syscall");
            exit(EXIT_FAILURE);
        }
        
        pthread_t tid;
        struct fd_info obj;
        obj.addr=cli_addr;
        obj.fd=newsockfd;
        obj.file_fd=fd;
        pthread_create(&tid, NULL, handleAccept, &obj);
        pthread_join(tid, NULL);

	    i++;
    }
    return 0;
}


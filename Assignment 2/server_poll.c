#include <stdio.h>
#include <string.h> 
#include <stdlib.h>
#include <errno.h>
#include <unistd.h> 
#include <arpa/inet.h> 
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <fcntl.h>
#include <poll.h>

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

int setup_socket()
{
    /* Function to handle creation and binding of socket to localhost IP and Port */
    int server_fd=socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd<0)
    {
        perror("Error in socket() syscall");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in serv_addr;
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_port=htons(8080);
    serv_addr.sin_addr.s_addr=INADDR_ANY;

    int b=bind(server_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    if (b<0)
    {
        perror("Error in bind() syscall");
        exit(EXIT_FAILURE);
    }

    int l=listen(server_fd, 12);
    if (l<0)
    {
        perror("Error in listen() syscall");
        exit(EXIT_FAILURE);
    }
    
    return server_fd;
}

int main(int argc, char const *argv[])
{
    struct sockaddr_in new_addr;
    int server_fd, ret_val, num_of_connections=15;
    socklen_t addrlen;
    char buf[256];
    int all_connections[num_of_connections];
    struct pollfd *pfds;
    int nfds=14, num_open_fds=nfds;
    struct pollfd pollfds[num_of_connections+1];
    char newline_char='\n', space_char=' ', colon_char=':';

    int fd = open("poll_server_file.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd<0)
    {
        perror("Error in open() syscall");
        exit(EXIT_FAILURE);
    }

    server_fd=setup_socket();
    
    pollfds[0].fd=server_fd;
    pollfds[0].events=POLL_IN;
    int useClient=0;

    for (int i = 1; i < 15; i++)
    {
        pollfds[i].fd=0;
        pollfds[i].events=POLL_IN;
    }

    while (1)
    {
        // Timeout set for 5000ms
        int poll_ret=poll(pollfds, useClient+1, 5000);
        if(poll_ret<0)
        {
            perror("Error in using poll() syscall");
            exit(EXIT_FAILURE);
        }

        // Checking if an input is observed in poll syscall which is returned using POLL_IN in revents
        if(pollfds[0].revents & POLL_IN)
        {
            int socket_accept_fd=accept(server_fd, (struct sockaddr*)&new_addr, &addrlen);
            if (socket_accept_fd<0)
            {
                perror("Error in using accept() syscall");
                exit(EXIT_FAILURE);
            }

            printf("\nServer Connected to: %s : %d\n", inet_ntoa(new_addr.sin_addr), ntohs(new_addr.sin_port));
            for (int i = 1; i < num_of_connections; i++)
            {
                if (pollfds[i].fd==0)
                {
                    // Connecting a new client
                    pollfds[i].fd=socket_accept_fd;
                    pollfds[i].events=POLL_IN;
                    useClient++;
                    break;
                }
            }
            if (poll_ret==1)
            {
                // If there was just one IO request to connect, we move to next iteration
                continue;
            }                        
        }

        for(int i=1; i<num_of_connections; ++i)
        {
            // Check if IO has occured
            if (pollfds[i].fd>0 && pollfds[i].revents & POLL_IN)
            {
                bzero(buf, 256);
                int rd_ret=read(pollfds[i].fd, buf, 255);
                if (rd_ret<0)
                {
                    perror("Error in read()");
                    exit(EXIT_FAILURE);
                }

                if(rd_ret==0){
                    // Read length 0 means client has disconnected
                    getpeername(pollfds[i].fd , (struct sockaddr*)&new_addr , (socklen_t*)&new_addr);
                    pollfds[i].fd=0;
                    pollfds[i].events=0;
                    pollfds[i].revents=0;
                    useClient--;
                }
                else if(strcmp(buf, "end")){
                    int x=atoi(buf);
                    printf("From Client: %d\n", x);
                    long long y=factorial(x);
                    char char_y[20];
                    sprintf(char_y, "%lld", y);
                    char* char_y1 = char_y;

                    strncat(char_y1, &newline_char, 1);
					send(pollfds[i].fd, char_y1 , strlen(char_y1) , 0);

                    char* char_y2=inet_ntoa(new_addr.sin_addr);
                    strncat(char_y2, &colon_char, 1);
                    int port_x=ntohs(new_addr.sin_port);
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

                    // send(pollfds[i].fd, buf, 255, 0);
                }
            }
        }
    }
    
    
    return 0;
}

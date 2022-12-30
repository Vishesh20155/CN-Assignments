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
    /* This function is used to setup a socket on localhost on the specified port */
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
    int serv_fd=setup_socket();
    char buffer[256];
    char newline_char='\n', space_char=' ', colon_char=':';

    int max_sd, sd;

    struct sockaddr_in new_addr;

    fd_set readfds; // Set of file descriptors for reading

    int no_of_clients=15, client_socket[15];
    // Socket ID can be 0, so change it to -1
    for (int i = 0; i < no_of_clients; i++)
	{
        // This is required to store all the FDs that are being used for the FD set- readfds
		client_socket[i] = 0;
	}

    int addrlen=sizeof(new_addr);


    int fd = open("select_server_file.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd<0)
    {
        perror("Error in open() syscall");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        // Remove all descriptors from the file descriptor set readfds
        FD_ZERO(&readfds);

        // Adding the socket fd as the first file descriptor in the set
        FD_SET(serv_fd, &readfds);
        max_sd=serv_fd;

        for (int i = 0; i < no_of_clients; i++)
        {
            sd=client_socket[i];
            if (sd>0)
            {
                // Adding the file descriptor for already connected clients in the set.
                FD_SET(sd, &readfds);
            }
            if(sd>max_sd)
            {
                max_sd=sd;
            }      
        }

        int sel_ret=select(max_sd+1, &readfds, NULL, NULL, NULL);

        // 
        if ((sel_ret<0) && (errno!=EINTR))
        {
            perror("Error in using select() syscall");
            exit(EXIT_FAILURE);
        }
        
        // Check if server FD still present in set
        if (FD_ISSET(serv_fd, &readfds))
        {
            int socket_accepted_fd=accept(serv_fd, (struct sockaddr*)&new_addr, (socklen_t*)&addrlen);
            if (socket_accepted_fd<0)
            {
                perror("Error in accept() syscall");
                exit(EXIT_FAILURE);
            }

			printf("\nNew connection from IP : %s, port : %d\n"  , inet_ntoa(new_addr.sin_addr), ntohs(new_addr.sin_port));

            // Incoming client being assigned to one of the free CLIENTS
            for (int i = 0; i < no_of_clients; i++)
            {
                if (client_socket[i]==0)
                {
                    client_socket[i]=socket_accepted_fd;

                    break;
                }
            }        
        }

        for (int  i = 0; i < no_of_clients; i++)
        {
            sd=client_socket[i];
            // Check if any of the clients are ready for IO operation, i.e. have sent an input
            if (FD_ISSET(sd, &readfds))
            {
                bzero(buffer, 256);
                int rd_ret = read(sd, buffer, 256);
                if (rd_ret<0)
                {
                    perror("Error in Read() syscall");
                    exit(EXIT_FAILURE);
                }
                
                if (rd_ret == 0)
                {
                    //Somebody disconnected , get his details and print
                    // getpeername(sd , (struct sockaddr*)&new_addr , (socklen_t*)&new_addr);
                    // printf("\nHost disconnected , ip %s , port %d \n\n" , inet_ntoa(new_addr.sin_addr) , ntohs(new_addr.sin_port));
                        
                    //Close the socket and mark as 0 in list for reuse
                    int cl_ret=close(sd);
                    if (cl_ret<0)
                    {
                        perror("Error in using close() syscall for socket");
                        exit(EXIT_FAILURE);
                    }
                    
                    client_socket[i] = 0;
                }
                else if(strcmp(buffer, "end"))
				{
                    int x=atoi(buffer);
                    printf("Received at server: %d\n", x);
                    long long y=factorial(x);
                    char char_y[20];
                    sprintf(char_y, "%lld", y);
                    char* char_y1 = char_y;
                    strncat(char_y1, &newline_char, 1);
					send(sd , char_y1 , strlen(char_y1) , 0);

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

				}
            }
        }
        
        
    }

    close(fd);
    return 0;
}

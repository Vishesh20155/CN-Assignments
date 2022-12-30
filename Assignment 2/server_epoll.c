#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h> 
#include <netinet/in.h>
#include <sys/time.h>


long long factorial(int x)
{
  long long ans = 1;
  int i = 2;
  while (i <= x)
  {
    ans = ans * i;
    i++;
  }

  return ans;
}

int create_and_bind()
{
  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd < 0)
  {
    perror("Error in socket() syscall");
    exit(EXIT_FAILURE);
  }

  struct sockaddr_in serv_addr;
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(8080);
  serv_addr.sin_addr.s_addr = INADDR_ANY;

  int b = bind(server_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
  if (b < 0)
  {
    perror("Error in bind() syscall");
    exit(EXIT_FAILURE);
  }

  return server_fd;
}

int make_socket_non_blocking(int sfd)
{
  int flags, s;

  flags = fcntl(sfd, F_GETFL, 0);
  if (flags == -1)
  {
    perror("Error in using fcntl()");
    exit(EXIT_FAILURE);
  }

  flags |= O_NONBLOCK;
  s = fcntl(sfd, F_SETFL, flags);
  if (s == -1)
  {
    perror("Error in using fcntl()");
    exit(EXIT_FAILURE);
  }

  return 0;
}

#define MAXEVENTS 15
#define PORT 8080

int main(int argc, char *argv[])
{
  int sfd, nfds;
  struct epoll_event ev;
  struct epoll_event events[MAXEVENTS];
  struct sockaddr_in addr;
  socklen_t addrlen;

  sfd = create_and_bind();

  char fact[20][20] = {"1", "2", "6", "24", "120", "720", "5040", "40320", "362880", "3628800", "39916800", "479001600", "6227020800", "87178291200", "1307674368000", "20922789888000", "355687428096000", "6402373705728000", "121645100408832000", "2432902008176640000"};
  char newline_char='\n', space_char=' ', colon_char=':';
  int fd = open("epoll_server_file.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if (fd < 0)
  {
    perror("Error in open() syscall");
    exit(EXIT_FAILURE);
  }
  // make_socket_non_blocking (sfd);

  int lis_ret = listen(sfd, MAXEVENTS);
  if (lis_ret < 0)
  {
    perror("Error in using listen syscall.");
    exit(EXIT_FAILURE);
  }

  int epollfd = epoll_create1(0);
  if (epollfd == -1)
  {
    perror("Error in using epoll_create1 syscall.");
    exit(EXIT_FAILURE);
  }

  ev.data.fd = sfd;
  ev.events = EPOLLIN;
  int ctl_ret = epoll_ctl(epollfd, EPOLL_CTL_ADD, sfd, &ev);
  if (ctl_ret < 0)
  {
    perror("Error in epoll_ctl: listen_sock");
    exit(EXIT_FAILURE);
  }

  while (1)
  {
    nfds = epoll_wait(epollfd, events, MAXEVENTS, -1);
    if (nfds == -1)
    {
      perror("epoll_wait");
      exit(EXIT_FAILURE);
    }

    for (int n = 0; n < nfds; ++n)
    {
      if (events[n].data.fd == sfd)
      {
        int conn_sock = accept(sfd, (struct sockaddr *)&addr, &addrlen);
        if (conn_sock == -1)
        {
          perror("accept");
          exit(EXIT_FAILURE);
        }
        make_socket_non_blocking(conn_sock);
        ev.events = EPOLLIN | EPOLLET;
        ev.data.fd = conn_sock;
        if (epoll_ctl(epollfd, EPOLL_CTL_ADD, conn_sock, &ev) == -1)
        {
          perror("epoll_ctl: conn_sock");
          exit(EXIT_FAILURE);
        }
      }
      else
      {
        char buffer[256];
        read(events[n].data.fd, buffer, 255);
        if (strcmp(buffer, "end") == 0)
        {
          printf("\n");
          continue;
        }
        printf("Read in Server: %s\n", buffer);

        int x = atoi(buffer);
        // char y=fact[x-1];
        // printf("Computed in Server: %d\n", x);
        // write(events[n].data.fd, fact[x-1], strlen(fact[x-1]));
        int wr_ret = write(events[n].data.fd, fact[x - 1], strlen(fact[x - 1]));
        if (wr_ret < 0)
        {
          perror("Error in write()");
          exit(EXIT_FAILURE);
        }
        char* y=inet_ntoa(addr.sin_addr);
        char char_port[256];
        
        sprintf(char_port, "%d", ntohs(addr.sin_port));

        strncat(char_port, &space_char, 1);
        strncat(char_port, fact[x-1], strlen(fact[x-1]));
        strncat(char_port, &newline_char, 1);
        strncat(y, &colon_char, 1);
        strncat(y, char_port, strlen(char_port));
        wr_ret=write(fd, y, strlen(y));
      }
    }
  }

  close(fd);
  return EXIT_SUCCESS;
}
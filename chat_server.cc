#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <sys/errno.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <sys/stat.h>

#include <stdarg.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <iostream>
#include <sstream>
#include <string>

#define BUFSIZE 2048

using namespace std;

class Server{
    int echo(int fd);
  public: 
    int start_server(int tcpsocket);
};

int Server::start_server(int tcpsocket){
  struct sockaddr_in fsin;  /* the from address of a client */
  int msock;      /* master server socket   */
  fd_set  rfds;     /* read file descriptor set */
  fd_set  afds;     /* active file descriptor set */
  unsigned int  alen;   /* from-address length    */
  int fd, nfds;
  
  msock = tcpsocket;
  nfds = getdtablesize();
  FD_ZERO(&afds);
  FD_SET(msock, &afds);


  cout << "Starting server" << endl;
  
  while (1) {
    memcpy(&rfds, &afds, sizeof(rfds));

    if (select(nfds, &rfds, (fd_set *)0, (fd_set *)0, (struct timeval *)0) < 0)
      printf("select: %s\n", strerror(errno));
    if (FD_ISSET(msock, &rfds)) {
      int ssock;

      alen = sizeof(fsin);
      ssock = accept(msock, (struct sockaddr *)&fsin, &alen);
      if (ssock < 0)
        printf("accept: %s\n", strerror(errno));
      FD_SET(ssock, &afds);
    }

    for (fd=0; fd<nfds; ++fd)
      if (fd != msock && FD_ISSET(fd, &rfds))
        if (echo(fd) == 0) {
          (void) close(fd);
          FD_CLR(fd, &afds);
        }
  }
}

int Server::echo(int fd){
  cout << "Echo" << endl;
}

int main(int argc, char *argv[]) {
  stringstream strValue; 
  int msock;
  Server server;

  strValue << argv[1];
  strValue >> msock;

  server.start_server(msock);

  return 0;
}


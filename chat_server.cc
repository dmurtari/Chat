#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <sys/errno.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
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
  /*struct sockaddr_in fsin;  
  int msock;                
  fd_set  rfds;             
  fd_set  afds;            
  unsigned int  alen;       
  int fd, nfds, cc;
  char buf[BUFSIZE];
  
  msock = tcpsocket;
  nfds = getdtablesize();
  FD_ZERO(&afds);
  FD_SET(msock, &afds); */


  int svc;        /* listening socket providing service */
  int rqst;       /* socket accepting the request */
  int port;
  socklen_t alen;       /* length of address structure */
  struct sockaddr_in my_addr;    /* address of this service */
  struct sockaddr_in sin;
  struct sockaddr_in client_addr;  /* client's address */
  int sockoptval = 1;
  char hostname[128]; /* host name, for debugging */

  cout << "Socket is " << tcpsocket << endl;

  /* loop forever - wait for connection requests and perform the service */
  alen = sizeof(client_addr);     /* length of address */

  for (;;) {
    while ((rqst = accept(tcpsocket, (struct sockaddr *)&client_addr, &alen)) < 0) {
      /* we may break out of accept if the system call */
      /* was interrupted. In this case, loop back and */
      /* try again */
      if ((errno != ECHILD) && (errno != ERESTART) && (errno != EINTR)) {
        perror("accept failed");
        exit(1);
      }
    }

    printf("received a connection from: %s port %d\n",
      inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
          shutdown(rqst, 2);    /* close the connection */
  }

/*
  while (1) {

    cout << "Copying" << endl;
    memcpy(&rfds, &afds, sizeof(rfds));

    int nbytes = read(msock, buf, BUFSIZE);
    cout << "Buf is: " << buf << endl;

    /*
    cout << "Selecting" << endl;
    if (select(nfds, &rfds, (fd_set *)0, (fd_set *)0, (struct timeval *)0) < 0)
      printf("select: %s\n", strerror(errno));

    cout << "Accepting" << endl;

    if (FD_ISSET(msock, &rfds)) {
      int ssock;

      alen = sizeof(fsin);
      ssock = accept(msock, (struct sockaddr *)&fsin, &alen);
      if (ssock < 0)
        printf("accept: %s\n", strerror(errno));
      FD_SET(ssock, &afds);
    }

    cout << "reading" << endl;
    cc = read(msock, buf, sizeof buf);
    cout << "BUFFER IS: " << buf << endl;

    /*
    for (fd=0; fd<nfds; ++fd)
      if (fd != msock && FD_ISSET(fd, &rfds))
        if (echo(fd) == 0) {
          (void) close(fd);
          FD_CLR(fd, &afds);
        }
        
  }*/
}

int Server::echo(int fd){
  cout << "Echo" << endl;
}

int main(int argc, char *argv[]) {
  stringstream strValue; 
  int msock;
  Server server;

  cout << "argv is " << argv[0]  << endl;

  strValue << argv[0];
  strValue >> msock;

  server.start_server(msock);

  return 0;
}


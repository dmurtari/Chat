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
  char buf[BUFSIZE];
  struct sockaddr_in fsin;  /* the from address of a client */
  int msock, ssock;      /* master server socket   */
  fd_set  rfds;     /* read file descriptor set */
  fd_set  afds;     /* active file descriptor set */
  unsigned int  alen;   /* from-address length    */
  int fd, nfds;

  msock = tcpsocket;

  nfds = getdtablesize();
  FD_ZERO(&afds);
  FD_SET(msock, &afds);

  while (1) {
    memcpy(&rfds, &afds, sizeof(rfds));

    if (select(nfds, &rfds, (fd_set *)0, (fd_set *)0,
        (struct timeval *)0) < 0)
      printf("select: %s\n", strerror(errno));
    if (FD_ISSET(msock, &rfds)) {
      alen = sizeof(fsin);
      ssock = accept(msock, (struct sockaddr *)&fsin, &alen);
      if (ssock < 0)
        printf("accept: %s\n", strerror(errno));
      FD_SET(ssock, &afds);
    }
    for (fd=0; fd<nfds; ++fd){
      if (fd != msock && FD_ISSET(fd, &rfds)){
        int nbytes = read(ssock, buf, BUFSIZE);
        cout << "Read " << nbytes << " bytes and Buf is: " << buf << endl;  
      }    
    }
  }
  
  /*
  char buf[BUFSIZE];
  int svc;        
  int rqst;      
  int port;
  socklen_t alen;       
  struct sockaddr_in my_addr;   
  struct sockaddr_in sin;
  struct sockaddr_in client_addr;  
  int sockoptval = 1;
  char hostname[128]; 



  cout << "Socket is " << tcpsocket << endl;

  
  alen = sizeof(client_addr);    

  while (true) {
    cout << "Looping again" << endl;
    rqst = accept(tcpsocket, (struct sockaddr *)&client_addr, &alen);

    printf("received a connection from: %s port %d\n",
      inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
          shutdown(rqst, 2);    

    while (true){
      int nbytes = read(rqst, buf, BUFSIZE);
      cout << "Read " << nbytes << " bytes and Buf is: " << buf << endl;
    }
  }
  */
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


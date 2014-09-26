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
#include <string>

#define BUFSIZE 2048

using namespace std;

class Server{
    uint16_t passivesock();
  public:
    int start_server();
};

int Server::start_server(){
  char  *portnum = "5004";  
  struct sockaddr_in fsin; 
  char buf[BUFSIZE];

  uint16_t port = passivesock();
  sprintf(buf, "%d", port);

  mkfifo("/tmp/myFIFO", 0666);
  int pipe = open("/tmp/myFIFO", O_WRONLY);
  write(pipe, buf, sizeof(buf));
  close(pipe);
  
  while (true) {
    
  }

  return 0;
}

uint16_t Server::passivesock(){
  struct sockaddr_in sin; /* an Internet endpoint address  */
  int     s;              /* socket descriptor             */

  memset(&sin, 0, sizeof(sin));
  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = INADDR_ANY;
  sin.sin_port = htons(0);

  /* Allocate a socket */
  s = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (s < 0)
    cout << "can't create socket" << endl;

  /* Bind the socket */
  if (bind(s, (struct sockaddr *)&sin, sizeof(sin)) < 0){
    cout << "binding failed" << endl;
  }

  socklen_t socklen = sizeof(sin);
  getsockname(s, (struct sockaddr *)&sin, &socklen);
  s = ntohs(sin.sin_port);

  cout << "bound to port " << s << endl;
  return s;
}

int main(int argc, char *argv[]){
  Server server;
  server.start_server();
  return 0;
}

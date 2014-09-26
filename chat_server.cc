#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <sys/errno.h>
#include <netinet/in.h>
#include <netdb.h>

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

};

int main(int argc, char *argv[]){
  char  *portnum = "5004";  
  struct sockaddr_in fsin; 
  int msock;
  fd_set  rfds;
  fd_set  afds;
  unsigned int  alen;
  int fd, nfds;
  string buf;

  buf = argv[0];
  cout << "In server, buf is " << buf << endl;
  
  while (1) {
    return 0;
  }
}
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <sys/errno.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#include <stdarg.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <string>
#include <map>
#include <iostream>
#include <vector>

using namespace std;

#define SERVICE_PORT  21234
#define BUFSIZE 2048

void Print (const vector<string>& v);

class Coordinator{
  public:
    int start_coordinator();
    void start_chat(string msg);
};

int Coordinator::start_coordinator(){
  struct sockaddr_in myaddr;  
  struct sockaddr_in remaddr; 
  socklen_t addrlen = sizeof(remaddr);    
  int recvlen; 
  int fd;
  int msgcnt = 0;
  char buf[BUFSIZE];
  vector<string> msg;
  map<string, string> sessions;
  char * pch;

  if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("cannot create socket\n");
    return 0;
  }

  memset((char *)&myaddr, 0, sizeof(myaddr));
  myaddr.sin_family = AF_INET;
  myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  myaddr.sin_port = htons(SERVICE_PORT);

  if (bind(fd, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0) {
    perror("bind failed");
    return 0;
  }

  while (true) {
    printf("waiting on port %d\n", SERVICE_PORT);
    recvlen = recvfrom(fd, buf, BUFSIZE, 0, (struct sockaddr *)&remaddr, 
                       &addrlen);
    if (recvlen > 0) {
      buf[recvlen] = 0;
      printf("received message: \"%s\" (%d bytes)\n", buf, recvlen);
    }
    else{
      printf("uh oh - something went wrong!\n");
    } 

    pch = strtok (buf," ,.-");
    while (pch != NULL){
      msg.push_back(pch);
      pch = strtok (NULL, " ,.-");
    }

    if (msg[0] == "Start"){
      cout << "Starting chat " << msg[1] << endl;
      start_chat(msg[1]);
    } else if (msg[0] == "Find"){
      cout << "Finding chat " << msg[1] << endl;
    } else if (msg[0] == "Terminate"){
      cout << "Terminating chat " << msg[1] << endl;
    } else {
      cout << "Command not recognized" << endl;
    }

    printf("sending response \"%s\"\n", buf);
    
    if (sendto(fd, buf, 20, 0, (struct sockaddr *)&remaddr, addrlen) < 0){
      perror("sendto");
    }
  }

  return 0;
}

int main(int argc, char **argv) {
  Coordinator coordinator;
  coordinator.start_coordinator();
  return 0;
}

void Print (const vector<string>& v){
  //vector<int> v;
  for (int i=0; i<v.size();i++){
    cout << v[i] << " ";
  }
}


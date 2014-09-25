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
#include <sstream>
#include <iostream>
#include <vector>

using namespace std;

#define SERVICE_PORT  21234
#define MAXLEN 80
#define BUFSIZE 2048

void Print (const vector<string>& v);

class Coordinator{
    map<string, string> sessions;
  public:
    int start_coordinator();
    string start_chat(string msg);
    int passivesock();
};

int Coordinator::start_coordinator(){
  struct sockaddr_in myaddr;  
  struct sockaddr_in remaddr; 
  socklen_t addrlen = sizeof(remaddr);    
  int recvlen, msglen; 
  int fd, i;
  int msgcnt = 0;
  string result;
  vector<string> msg;
  char * pch;
  char buf[BUFSIZE];

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

    stringstream ss(buf);
    while (ss >> buf)
        msg.push_back(buf);
    fill_n(buf, BUFSIZE, NULL); 

    if (msg[0] == "Start"){
      cout << "Starting chat " << msg[1] << endl;
      cout << start_chat(msg[1]) << endl;
    } else if (msg[0] == "Find"){
      cout << "Finding chat " << msg[1] << endl;
    } else if (msg[0] == "Terminate"){
      cout << "Terminating chat " << msg[1] << endl;
    } else {
      cout << "Command not recognized" << endl;
    }
    
    msglen = result.copy(buf, result.length(), 0);
    printf("sending response \"%s\"\n", buf);

    if (sendto(fd, buf, MAXLEN, 0, (struct sockaddr *)&remaddr, addrlen) < 0){
      perror("sendto");
    }
    fill_n(buf, BUFSIZE, NULL); 
    msg.clear();

  }
  return 0;
}

string Coordinator::start_chat(string msg){
  if (sessions.count(msg)){
    return "-1";
  } else {
    sessions[msg] = passivesock();
    return sessions[msg];
  }
}

int Coordinator::passivesock(){
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

  cout << "bound to port " << s << endl;
  return s;
}

int main(int argc, char **argv) {
  Coordinator coordinator;
  coordinator.start_coordinator();
  return 0;
}

void Print (const vector<string>& v){
  for (int i=0; i<v.size();i++){
    cout << v[i] << " ";
  }
}


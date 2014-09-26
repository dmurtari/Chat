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

#include <string>
#include <map>
#include <sstream>
#include <iostream>
#include <vector>

using namespace std;

#define SERVICE_PORT  21234
#define MAXLEN 80
#define BUFSIZE 2048
#define QUEUE_LEN 32

void Print (const vector<string>& v);

class Coordinator{
    map<string, uint16_t> sessions;
  public:
    int start_coordinator();
    uint16_t start_chat(string chat_name);
    uint16_t find_chat(string chat_name);
    int passivesock();
};

int Coordinator::start_coordinator(){
  struct sockaddr_in myaddr;  
  struct sockaddr_in remaddr; 
  socklen_t addrlen = sizeof(remaddr);    
  int recvlen, msglen; 
  int fd, i;
  int msgcnt = 0;
  uint16_t result;
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
      result = start_chat(msg[1]);
    } else if (msg[0] == "Find"){
      result = find_chat(msg[1]);
    } else if (msg[0] == "Terminate"){
      cout << "Terminating chat " << msg[1] << endl;
    } else {
      cout << "Command not recognized" << endl;
    }
    
    if (result == 0){
      sprintf(buf, "%d", -1);
    } else{
      sprintf(buf, "%d", result);
    }

    printf("sending response \"%s\"\n", buf);

    if (sendto(fd, buf, MAXLEN, 0, (struct sockaddr *)&remaddr, addrlen) < 0){
      perror("sendto");
    }
    fill_n(buf, BUFSIZE, NULL); 
    msg.clear();

  }
  return 0;
}

uint16_t Coordinator::start_chat(string chat_name){
  pid_t pid;
  int tcp_sock;
  struct sockaddr_in sin;
  char arg[BUFSIZE];
  char buf1[BUFSIZE];

  if (sessions.count(chat_name)){
    cout << "Chat " << chat_name << " already exists." << endl;
    return 0;
  } else {
    tcp_sock = passivesock();
    if((pid = fork()) < 0)
      cout << "Fork failed" << endl;
    if (pid == 0){
      sprintf(arg, "%d", tcp_sock);
      cout << "Started socket " << arg << endl;
      if(execl("chat_server", arg, NULL) < 0)
        cout << "Exec failed" << endl;
    }

    int socklen = sizeof(sin);
    if (getsockname(tcp_sock, (struct sockaddr *)&sin,  (socklen_t*)&socklen) < 0) {
        printf("getsockname: %s\n", strerror(errno));
    }

    sessions[chat_name] = ntohs(sin.sin_port);
    return sessions[chat_name];
  }
}

uint16_t Coordinator::find_chat(string chat_name){
  if (sessions.count(chat_name)){
    cout << "Found chat " << chat_name << endl;
    return sessions[chat_name];
  } else {
    cout << "Can't find chat " << chat_name << endl;
    return 0;
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

  if (listen(s, QUEUE_LEN) < 0)
    printf("can't listen on %d port: %s\n", sin.sin_port, strerror(errno));

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


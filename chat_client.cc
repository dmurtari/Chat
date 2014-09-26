#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h> 

#include <string>
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

#define BUFSIZE 2048
#define MAXLEN 80
#define SERVICE_PORT  21234

int main(int argc, char *argv[]) {
  struct sockaddr_in myaddr, remaddr;
  string msg;
  int port;
  int fd, i;
  socklen_t slen=sizeof(remaddr);
  char buf[BUFSIZE], buf1[BUFSIZE];
  int recvlen;
  vector<string> msg_vec;
  size_t msglen;
  char *coordinator = argv[1];
  stringstream strValue; 

  strValue << argv[2];
  strValue >> port;

  if ((fd=socket(AF_INET, SOCK_DGRAM, 0))==-1)
    printf("socket created\n");

  memset((char *)&myaddr, 0, sizeof(myaddr));
  myaddr.sin_family = AF_INET;
  myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  myaddr.sin_port = htons(0);

  if (bind(fd, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0) {
    perror("bind failed");
    return 0;
  }       

  memset((char *) &remaddr, 0, sizeof(remaddr));
  remaddr.sin_family = AF_INET;
  remaddr.sin_port = htons(port);
  
  if (inet_aton(coordinator, &remaddr.sin_addr)==0) {
    fprintf(stderr, "inet_aton() failed\n");
    exit(1);
  }

  while (true) {
    cout << "> "; 
    getline(cin, msg);
    msglen = msg.copy(buf, msg.length(), 0);
    msglen = msg.copy(buf1, msg.length(), 0);

    stringstream ss(buf1);
    while (ss >> buf1)
      msg_vec.push_back(buf1);
    fill_n(buf1, BUFSIZE, NULL); 

    if (msg_vec[0] == "Start"){
      cout << "Got a start";
    } else if (msg_vec[0] == "Find"){
      cout << "Got a find";
    } else if (msg_vec[0] == "Terminate"){
      cout << "Terminating chat " << msg_vec[1] << endl;
    } else {
      cout << "Command not recognized" << endl;
    }

    if (sendto(fd, buf, MAXLEN, 0, (struct sockaddr *)&remaddr, slen)==-1) {
      perror("sendto");
      exit(1);
    }

    recvlen = recvfrom(fd, buf, BUFSIZE, 0, (struct sockaddr *)&remaddr, &slen);
    if (recvlen >= 0) {
      buf[recvlen] = 0;
      printf("Coordinator received message: \"%s\"\n", buf);
    }
    fill_n(buf, msglen, NULL); 
  }
  close(fd);
  return 0;
}
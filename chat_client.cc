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

using namespace std;

#define BUFSIZE 2048
#define MAXLEN 80
#define SERVICE_PORT  21234

int main(void) {
  struct sockaddr_in myaddr, remaddr;
  string msg;
  int fd, i;
  socklen_t slen=sizeof(remaddr);
  char buf[BUFSIZE];
  int recvlen;
  size_t msglen;
  char *coordinator = "127.0.0.1";

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
  remaddr.sin_port = htons(SERVICE_PORT);
  if (inet_aton(coordinator, &remaddr.sin_addr)==0) {
    fprintf(stderr, "inet_aton() failed\n");
    exit(1);
  }

  while (true) {
    cout << "> "; 
    getline(cin, msg);
    msglen = msg.copy(buf, msg.length(), 0);
    
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
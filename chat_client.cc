#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h> 

#define BUFLEN 2048
#define MSGS 5
#define SERVICE_PORT  21234

int main(void)
{
  struct sockaddr_in myaddr, remaddr;
  int fd, i;
  socklen_t slen=sizeof(remaddr);
  char buf[BUFLEN];
  int recvlen;
  char *server = "127.0.0.1";

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
  if (inet_aton(server, &remaddr.sin_addr)==0) {
    fprintf(stderr, "inet_aton() failed\n");
    exit(1);
  }

  for (i=0; i < MSGS; i++) {

    printf("Sending packet %d to %s port %d\n", i, server, SERVICE_PORT);
    sprintf(buf, "This is packet %d", i);
    if (sendto(fd, buf, 20, 0, (struct sockaddr *)&remaddr, slen)==-1) {
      perror("sendto");
      exit(1);
    }

    recvlen = recvfrom(fd, buf, BUFLEN, 0, (struct sockaddr *)&remaddr, &slen);
                if (recvlen >= 0) {
                        buf[recvlen] = 0; /* expect a printable string - terminate it */
                        printf("received message: \"%s\"\n", buf);
                }
  }
  close(fd);
  return 0;
}
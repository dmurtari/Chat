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
#include <vector>
#include <map>

#define BUFSIZE 2048

using namespace std;

void Print (const vector<string>& v);

class Server{
    int echo(int fd);
    vector<string> msgs;
    vector<string> msg;
    map<int, int> clients;
  public: 
    int start_server(int tcpsocket);
    string combine_msg(vector<string> msg);
};

int Server::start_server(int tcpsocket){
  char buf[BUFSIZE], sendbuf[BUFSIZE];
  struct sockaddr_in fsin;  /* the from address of a client */
  int msock, ssock;      /* master server socket   */
  fd_set  rfds;     /* read file descriptor set */
  fd_set  afds;     /* active file descriptor set */
  unsigned int  alen;   /* from-address length    */
  int fd, nfds;
  int timeout = 1;
  string result;
  string command, char_count;
  socklen_t slen=sizeof(fsin);
  struct timeval tv;
  msock = tcpsocket;

  nfds = getdtablesize();
  FD_ZERO(&afds);
  FD_SET(msock, &afds); 

  while (1) {
    struct timeval tv;
    memcpy(&rfds, &afds, sizeof(rfds));

    tv.tv_sec = 10;
    tv.tv_usec = 0;

    if (((timeout = select(nfds, &rfds, (fd_set *)0, (fd_set *)0, &tv)) < 0))
      printf("select: %s\n", strerror(errno));

    if (FD_ISSET(msock, &rfds)) {
      alen = sizeof(fsin);
      ssock = accept(msock, (struct sockaddr *)&fsin, &alen);
      if (ssock < 0)
        printf("accept: %s\n", strerror(errno));
      FD_SET(ssock, &afds);

      clients[ssock] = 0;
    }

    for (fd=0; fd<nfds; ++fd){
      if (timeout == 0){
        cout << fd << " timed out" << endl;
        close(fd);
      }
      if (fd != msock && FD_ISSET(fd, &rfds)){
        int nbytes = read(ssock, buf, BUFSIZE);

        stringstream ss(buf);
        while (ss >> buf)
        msg.push_back(buf);
        fill_n(buf, BUFSIZE, NULL);

        command = msg[0];
        cout << "Received a " << command << " command" << endl;
        if (command == "Submit"){
          msgs.push_back(combine_msg(msg));
          Print(msgs);
        } else if (command == "GetNext"){
          if (clients[ssock] == msgs.size()){
            result = -1;
            cout << "-1" << endl;
          } else {
            result = msgs[clients[ssock]];
            cout << "Last message was: " << msgs[clients[ssock]];
            clients[ssock] += 1;
          }
          int msglen = result.copy(sendbuf, result.length(), 0);
          if (sendto(ssock, sendbuf, BUFSIZE, 0, (struct sockaddr *)&fsin, slen)==-1) {
            perror("sendto");
            exit(1);
          }        
        } else if (command == "GetAll"){
          int nummessages = msgs.size() - clients[ssock];
          cout << nummessages << " is nummessages" << endl;
          if (nummessages == 0){
            cout << "-1" << endl;
            int msglen = result.copy(sendbuf, result.length(), 0);
            if (sendto(ssock, sendbuf, BUFSIZE, 0, (struct sockaddr *)&fsin, slen)==-1) {
                perror("sendto");
                exit(1);
            }
          } else {
            cout << "1" << endl;
            char nummsgs[15];
            sprintf(nummsgs, "%d", nummessages);

            if (sendto(ssock, nummsgs, BUFSIZE, 0, (struct sockaddr *)&fsin, slen)==-1) {
                perror("sendto");
                exit(1);
            }

            for (int i = clients[ssock]; i < msgs.size(); i++){
              result = msgs[i];
              int msglen = result.copy(sendbuf, result.length(), 0);
              if (sendto(ssock, sendbuf, BUFSIZE, 0, (struct sockaddr *)&fsin, slen)==-1) {
                perror("sendto");
                exit(1);
              }
              fill_n(sendbuf, BUFSIZE, NULL); 
            }
            clients[ssock] = msgs.size();
          }
        } else if (command == "Leave") {
          clients.erase(ssock);
          close(ssock);
        }

        fill_n(sendbuf, BUFSIZE, NULL); 
        msg.clear();
      }    
    }
  }
}

string Server::combine_msg(vector<string> msg){
  string result = "";
  for (int i=2; i < msg.size(); i++){
    result = result + msg[i];
  }

  return result;
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

void Print (const vector<string>& v){
  for (int i=0; i<v.size();i++){
    cout << v[i] << " ";
  }
}
#include "webserve.h"
#include <asm-generic/socket.h>
#include <cstdio>
#include <cstring>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <thread>
#include <unistd.h>

#define SA struct sockaddr

webserve::webserve(std::function<void(recv_cb_t)> cb, int port)
    : port(port), recv_cb(cb) {
  int conn_fd;
  struct sockaddr_in addr, cli;
  int opt = 1;
  int addrlen = sizeof(cli);

  // socket create
  if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
    perror("socket failed");
  }

  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(port);

  // binding socket to given IP and verification
  if ((bind(server_socket, (SA *)&addr, sizeof(addr))) != 0) {
    perror("socket bind failed...\n");
  } else
    printf("Socket bind successful.... \n");
}

webserve::~webserve() { close(server_socket); }

void webserve::start() {
  std::thread listen_thread([this]() { listen_loop(); });

  listen_thread.join();
}
  
std::vector<std::string> webserve::split_string(std::string str, std::string delim){
  std::vector<std::string> tokens;
  std::string token;
  size_t pos = 0;
  size_t start = 0;

  while((pos = str.find(start)) != std::string::npos){
    token = str.substr(start, pos);

    start = pos + delim.length();
    tokens.push_back(token);
  }

  return tokens;
}

bool webserve::contains(std::string str, std::string token){
  return str.find(token) != std::string::npos;
}

void webserve::handle_client(int client_fd) {
  char buffer[1024] = {0};
  int valread = read(client_fd, buffer, 1024);
  printf("%s\n", buffer);

  recv_cb_t cb;
  cb.client_fd = client_fd;

  std::vector<std::string> req_tokens = split_string(buffer, "\r\n");
  for(auto t : req_tokens){
    if(contains(t, "HTTP/"))//this is the request line
    {
      std::vector<std::string> split_line = split_string(t, " ");

      cb.type = split_line[0];
      cb.path = split_line[1];
      cb.protocal = split_line[2];
    }else{
      cb.headers.push_back(t);//ERROR: probably needs to be a map
    }
  }

  recv_cb(cb);

  close(client_fd);
}

void webserve::listen_loop() {
  struct sockaddr_in addr;
  int addrlen = sizeof(addr);

  if ((listen(server_socket, 5)) != 0) {
    perror("Listen Failed....");
  }

  while (running) {
    int conn_fd = accept(server_socket, (SA *)&addr, (socklen_t *)&addrlen);

    std::thread client_thread([this, conn_fd]() { handle_client(conn_fd); });
    client_thread.detach();
  }
}

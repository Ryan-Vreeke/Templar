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

webserve::webserve(std::string pages, int port) : port(port), pages(pages) {
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
  running = true;
  std::thread listen_thread([this]() { listen_loop(); });

  listen_thread.join();
}

std::vector<std::string> webserve::split_string(std::string str, const std::string &delim) {
  std::vector<std::string> tokens;
  std::string token;

  size_t pos = 0;
  size_t start = 0;

  while ((pos = str.find(delim, start)) != std::string::npos) {
    token = str.substr(start, pos - start);

    start = pos + delim.length();
    tokens.push_back(token);
  }

  return tokens;
}

bool webserve::contains(std::string str, std::string token) {
  return str.find(token) != std::string::npos;
}

void webserve::handle_client(int client_fd) {
  char request[1024] = {0};
  int valread = read(client_fd, request, 1024);

  WebContext context{pages};
  context.client_fd = client_fd;

  std::vector<std::string> lines = split_string(request, "\r\n");
  std::vector<std::string> request_line = split_string(lines[0], " ");

  for(int i = 1; i<lines.size(); i++){
    std::vector<std::string> header_line = split_string(lines[i], ":");
    context.headers[header_line[0]] = header_line[1];
  }
  
  std::string response;
  if(request_line[0] == "GET"){
    response = get_map[request_line[1]](context);
  }else if(request_line[0] == "POST"){
    response = post_map[request_line[1]](context);
  }
  
  send(client_fd, response.c_str(), response.length(), 0);
  close(client_fd);
}


void webserve::listen_loop() {
  struct sockaddr_in addr;
  int addrlen = sizeof(addr);

  if ((listen(server_socket, 5)) != 0) {
    perror("Listen Failed....");
  }

  while (running) {
    printf("waiting");
    int conn_fd = accept(server_socket, (SA *)&addr, (socklen_t *)&addrlen);

    std::thread client_thread([this, conn_fd]() { handle_client(conn_fd); });
    client_thread.detach();
  }
}

void webserve::GET(std::string path, std::function<std::string(WebContext)> cb) {
  get_map[path] = cb;
}

void webserve::POST(std::string path, std::function<std::string(WebContext)> cb) {
  post_map[path] = cb;
}

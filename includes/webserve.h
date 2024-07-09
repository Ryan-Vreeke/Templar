#pragma once

#include <functional>
#include <map>
#include "recv_cb_t.h"


class webserve {

public:
  webserve(std::function<void(recv_cb_t)> cb, int port = 3000);
  webserve(webserve &&) = default;
  webserve(const webserve &) = default;
  webserve &operator=(webserve &&) = default;
  webserve &operator=(const webserve &) = default;
  ~webserve();

  void start();

private:
  std::function<void(recv_cb_t)> recv_cb;
  int port;
  int server_socket;
  bool running;
  std::map<int, struct sockaddr_in *> client_map;

  void listen_loop();
  void handle_client(int client_fd);
  std::vector<std::string> split_string(std::string str, std::string delim);
  bool contains(std::string str, std::string token);
};

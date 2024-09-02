#include "webserve.h"

#include <asm-generic/socket.h>
#include <cstddef>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstdio>
#include <cstring>
#include <format>
#include <thread>
#include <unordered_map>

#include "Watchman.h"
#include "WebContext.h"

#define SA struct sockaddr

webserve::webserve(std::string pages, int port)
    : port(port), pages(pages), templ(pages), file_watcher(pages) {
  int conn_fd;
  struct sockaddr_in addr, cli;
  int opt = 1;
  int addrlen = sizeof(cli);

  // socket create
  if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    perror("socket failed");

  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(port);

  // binding socket to given IP and verification
  if ((bind(server_socket, (SA *)&addr, sizeof(addr))) != 0) {
    perror("socket bind failed...\n");
    return;
  }

  printf("Socket bind successful.... \n");

  // making watchfolder
  file_watcher.init_watchman();
  file_watcher_cb = new watchman_cb();

  file_watcher_cb->created_cb = [&](const std::string &str) {
    file_created(str);
  };

  file_watcher_cb->delete_cb = [&](const std::string &str) {
    file_deleted(str);
  };

  file_watcher_cb->modify_cb = [&](const std::string &str) {
    file_modified(str);
  };

  file_watcher_cb->moved_cb = [&](const std::string &str, bool in) {
    file_moved(str, in);
  };
}

webserve::~webserve() {
  close(server_socket);
  file_watcher.stop();
  delete file_watcher_cb;
}

void webserve::start() {
  running = true;
  file_watcher.start_watch(file_watcher_cb);
  std::thread listen_thread([&]() { listen_loop(); });

  listen_thread.join();
}

std::vector<std::string> webserve::split_string(std::string str,
                                                const std::string &delim) {
  std::vector<std::string> tokens;
  std::string token;

  size_t start = 0;
  size_t pos = 0;

  if (delim.empty()) {
    throw std::invalid_argument("Delimiter cannot be empty");
  }

  while ((pos = str.find(delim, start)) != std::string::npos) {
    token = str.substr(start, pos - start);

    start = pos + delim.length();
    tokens.push_back(token);
  }

  token = str.substr(start);
  tokens.push_back(token);

  return tokens;
}

bool webserve::contains(std::string str, std::string token) {
  return str.find(token) != std::string::npos;
}

// FIX: doesn't stop thread correctly
void webserve::stop() { running = false; }

void webserve::add_headers(std::map<std::string, std::string> &headers,
                           std::vector<std::string> lines) {
  for (int i = 1; i < lines.size(); i++) {
    std::vector<std::string> header_line = split_string(lines[i], ":");
    if (header_line.size() < 2) {
      continue;
    }
    headers[header_line[0]] = header_line[1];
  }
}

void webserve::TrimPath(std::string &path) {
  if (path[path.length() - 1] == '/' && path.length() > 1) {
    path.erase(path.length() - 1);
  }
}

bool webserve::isPath(const std::string &path) {
  return (get_map.contains(path) || post_map.contains(path));
}

std::string webserve::send_file(std::string path, WebContext context) {
  std::string page = templ.load_file(path);
  std::string ret_code = "200 OK";
  if (page == "") {
    ret_code = "500 Internal Server Error";
  }

  return std::format(
      "HTTP/1.1 {}\r\nContent-Type:{}\r\nContent-Length:{}\r\n\r\n{}\r\n",
      ret_code, context.headers["Accept"], page.length(), page);
}

std::string webserve::buildResponse(const std::string &request, int client_fd) {
  std::string path;
  WebContext context{templ, client_fd};

  std::vector<std::string> lines = split_string(request, "\r\n");
  std::vector<std::string> request_line = split_string(lines[0], " ");

  if (request_line.size() < 2) {
    return std::format("HTTP/1.1 {}\r\n", "400 Bad Request");
  }

  path = request_line[1];
  TrimPath(path);
  add_headers(context.headers, lines);

  if (tmpp::isFile(templ.public_dir + path)) {
    return send_file(templ.public_dir + path, context);
  }

  if (!isPath(path)) {
    return std::format("HTTP/1.1 {}\r\nContent-Type:{}\r\n\r\n{}\r\n",
                       "404 Not Found", context.headers["Accept"],
                       "PAGE NOT FOUND");
  }

  return userCall(request_line[0], path, context);
}

std::string webserve::userCall(const std::string &type, const std::string &path,
                               WebContext &ctx) {

  std::unordered_map<std::string, std::function<std::string()>> command{
      {"GET", [&]() -> std::string { return get_map[path](ctx); }},
      {"POST", [&]() -> std::string { return post_map[path](ctx); }},
  };

  return command[type]();
}

void webserve::handle_client(int client_fd) {
  std::string response, request;
  request.resize(1024);

  int recv = read(client_fd, &request[0], request.size());
  if (recv == 0)
    return;

  request.resize(recv);
  response = buildResponse(request, client_fd);

  send(client_fd, response.c_str(), response.length(), 0);
}

void webserve::listen_loop() {
  struct sockaddr_in addr;
  int addrlen = sizeof(addr);

  if ((listen(server_socket, 5)) != 0) {
    perror("Listen Failed....");
  }

  while (running) {
    int conn_fd = accept(server_socket, (SA *)&addr, (socklen_t *)&addrlen);
    if (conn_fd == 0)
      continue;

    std::thread client_thread([this, conn_fd]() {
      handle_client(conn_fd);
      close(conn_fd);
    });

    client_thread.detach();
  }
}

void webserve::GET(std::string path,
                   std::function<std::string(WebContext)> cb) {
  get_map[path] = cb;
}

void webserve::POST(std::string path,
                    std::function<std::string(WebContext)> cb) {
  post_map[path] = cb;
}

void webserve::file_created(const std::string &str) {
  templ.add_file(templ.public_dir + "/" + str);
}

void webserve::file_deleted(const std::string &str) { templ.reload_defs(); }

void webserve::file_modified(const std::string &str) {
  templ.add_file(templ.public_dir + "/" + str);
}

void webserve::file_moved(const std::string &str, bool in) {
  if (in) {
    templ.add_file(templ.public_dir + "/" + str);
    return;
  }

  templ.reload_defs();
}

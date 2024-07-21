#pragma once

#include <functional>
#include <map>

#include "Watchman.h"
#include "WebContext.h"
#include "tmpp.h"

class webserve
{
 public:
	webserve(std::string pages, int port = 3000);
	webserve(webserve &&) = default;
	webserve(const webserve &) = default;
	webserve &operator=(webserve &&) = default;
	webserve &operator=(const webserve &) = default;
	~webserve();

	void start();
	void stop();
	void GET(std::string path, std::function<std::string(WebContext)> cb);
	void POST(std::string path, std::function<std::string(WebContext)> cb);

 private:
	int port;
	int server_socket;
	bool running;
	tmpp templ;
  Watchman file_watcher;
  watchman_cb *file_watcher_cb;
	std::string pages;
	std::map<std::string, std::function<std::string(WebContext)>> get_map;
	std::map<std::string, std::function<std::string(WebContext)>> post_map;

	void listen_loop();
	void handle_client(int client_fd);
	bool contains(std::string str, std::string token);
	void add_headers(std::map<std::string, std::string> &headers, std::vector<std::string> lines);
	bool isPath(std::string path);

  void file_created(const std::string& str);
  void file_deleted(const std::string& str);
  void file_modified(const std::string& str);
  void file_moved(const std::string& str, bool in);

	std::string send_file(std::string path, WebContext context);
	std::vector<std::string> split_string(std::string str, const std::string &delim);
};

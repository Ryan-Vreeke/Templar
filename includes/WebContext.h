#pragma once

#include "tmpp.h"
#include <map>
#include <string>
class WebContext {
public:
  std::map<std::string, std::string> headers;
  int client_fd;

  WebContext(std::string pages_folder);
  WebContext(WebContext &&) = default;
  WebContext(const WebContext &) = default;
  WebContext &operator=(WebContext &&) = default;
  WebContext &operator=(const WebContext &) = default;
  ~WebContext();

  template<typename T> std::string Render(int code, std::string page, T data);

private:
  tmpp templ;
  
  std::map<int, std::string> response_map = {
      {200, "200 OK"},           {201, "201 Created"},
      {204, "204 No Content"},   {400, "400 Bad Request"},
      {401, "401 Unauthorized"}, {403, "403 Forbidden"},
      {404, "404 Not Found"},    {500, "500 Internal Server Error"},
      {502, "502 Bad Gateway"},  {503, "503 Service Unavailable"}};
};


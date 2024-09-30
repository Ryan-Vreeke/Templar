#pragma once

#include <format>
#include <map>
#include <nlohmann/json.hpp>
#include <string>

#include "tmpp.h"
using json = nlohmann::json;
class WebContext {
public:
  std::map<std::string, std::string> headers;
  nlohmann::json body;
  int client_fd;

  WebContext(tmpp _templ);
  WebContext(tmpp _templ, int client_fd);
  WebContext(WebContext &&) = default;
  WebContext(const WebContext &) = default;
  WebContext &operator=(WebContext &&) = default;
  WebContext &operator=(const WebContext &) = default;
  ~WebContext();

  std::string Render(int code, std::string page, std::map<std::string, std::string> var);
  std::string Render(int code, std::string page, nlohmann::json json);
  std::string Render(int code, nlohmann::json json);
  std::string Render(int code, std::string page);
  std::string Render(int code);
  std::string Respond(int code, std::string response);
  std::string Respond(int code, nlohmann::json response);

private:
  tmpp templ;

  std::map<int, std::string> response_map = {
      {200, "200 OK"},           {201, "201 Created"},
      {204, "204 No Content"},   {400, "400 Bad Request"},
      {401, "401 Unauthorized"}, {403, "403 Forbidden"},
      {404, "404 Not Found"},    {500, "500 Internal Server Error"},
      {502, "502 Bad Gateway"},  {503, "503 Service Unavailable"}};
};

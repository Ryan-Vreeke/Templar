#include "WebContext.h"
#include <algorithm>
#include <format>
#include <iostream>

WebContext::WebContext(std::string pages_path) : templ(pages_path) {}

WebContext::~WebContext() {}

void clean_page(std::string &html) {
  html.erase(std::remove_if(html.begin(), html.end(),
                            [](char c) { return c == '\n' || c == '\t'; }),
             html.end());

  html.erase(std::unique(html.begin(), html.end(),
                         [](char a, char b) { return (a == ' ' && b == ' '); }),
             html.end());
}
 
std::string WebContext::Render(int code, std::string page) {

  std::string html = templ.block_contents[page];
  templ.prep_html(html);
  clean_page(html);

  std::string response =
      std::format("HTTP/1.1 {}\r\nContent-Type: {}\r\n{}\r\n\r\n",
                  response_map[code], "text/html; charset=utf-8", html);

  return response;
}

//   std::cout << "response: " <<  response << std::endl;

//   send(cb->client_fd, response.c_str(), response.length(), 0);
// }

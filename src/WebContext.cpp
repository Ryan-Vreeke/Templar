#include "WebContext.h"
#include <format>
#include <iostream>

WebContext::WebContext(std::string pages_path) : templ(pages_path) {}

WebContext::~WebContext() {}

std::string WebContext::Render(int code, std::string page) {

  std::string html = templ.block_contents[page];
  templ.prep_html(html);

  std::string response =
      std::format("HTTP/1.1 {}\r\nContent-Type: {}\r\n{}\r\n\r\n",
                  response_map[code], headers["Accept"], html);

  return response;
}

//   std::cout << "response: " <<  response << std::endl;

//   send(cb->client_fd, response.c_str(), response.length(), 0);
// }

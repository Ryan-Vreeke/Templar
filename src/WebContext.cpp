#include "WebContext.h"
#include <format>

WebContext::WebContext(tmpp _templ) : templ(_templ) {}

WebContext::~WebContext() {}

std::string WebContext::Render(int code, std::string page,
                               std::map<std::string, std::string> var) {
  if (!templ.block_contents.contains(page))
    return std::format("HTTP/1.1 {}\r\nContent-Type:{}\r\n\r\n{}\r\n",
                       response_map[code], "text/html; charset=UTF-8", page);

  std::string html = templ.block_contents[page];
  templ.prep_html(html);

  for (const auto &pair : var) {
    std::string variable = pair.first;
    if (variable[0] == '.')
      variable = variable.substr(1);

    templ.replace_var(html, variable, pair.second);
  }

  templ.replace_for(html);

  return std::format(
      "HTTP/1.1 {}\r\nContent-Type:{}\r\nContent-Lenght:{}\r\n\r\n{}\r\n",
      response_map[code], "text/html; charset=UTF-8", html.length(), html);
}

std::string WebContext::Render(int code, std::string page,
                               nlohmann::json json) {

  if (!templ.block_contents.contains(page))
    return std::format("HTTP/1.1 {}\r\nContent-Type:{}\r\n{}\r\n{}\r\n",
                       response_map[code], "text/html; charset=UTF-8",
                       "Access-Control-Allow-Origin= *", page);

  std::string html = templ.block_contents[page];
  templ.prep_html(html);

  for (json::iterator it = json.begin(); it != json.end(); it++) {
    std::string value;
    if (it->is_boolean()) {
      value = it->get<bool>() ? "true" : "false";
    } else {
      value = it->dump();
    }

    templ.replace_var(html, it.key(), value);
  }

  templ.replace_for(html);

  return std::format(
      "HTTP/1.1 {}\r\nContent-Type:{}\r\nContent-Lenght:{}\r\n\r\n{}\r\n",
      response_map[code], "text/html; charset=UTF-8", html.length(), html);
}

std::string WebContext::Render(int code, nlohmann::json json) {
  return std::format(
      "HTTP/1.1 {}\r\nContent-Type:application/json\r\n\r\n{}\r\n",
      response_map[code], json.dump());
}

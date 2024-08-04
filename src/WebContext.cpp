#include "WebContext.h"

WebContext::WebContext(tmpp _templ) : templ(_templ) {}

WebContext::~WebContext() {}

std::string WebContext::Render(int code, std::string page,
															 std::map<std::string, std::string> var)
{
	if (!templ.block_contents.contains(page))
		return std::format("HTTP/1.1 {}\r\nContent-Type:{}\r\n\r\n{}\r\n",
											 response_map[code], "text/html; charset=UTF-8", page);

	std::string html = templ.block_contents[page];
	templ.prep_html(html);

	for (const auto& pair : var)
	{
    std::string variable = pair.first;
    if(variable[0] == '.')
      variable = variable.substr(1);

		html = templ.replace_var(html, variable, pair.second);
	}

	return std::format(
			"HTTP/1.1 {}\r\nContent-Type:{}\r\nContent-Lenght:{}\r\n\r\n{}\r\n",
			response_map[code], "text/html; charset=UTF-8", html.length(), html);
}

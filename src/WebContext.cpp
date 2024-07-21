#include "WebContext.h"

#include <format>

WebContext::WebContext(tmpp _templ) : templ(_templ) {}

WebContext::~WebContext() {}

std::string WebContext::Render(int code, std::string page)
{
	if (!templ.block_contents.contains(page))
	{
		return std::format("HTTP/1.1 {}\r\nContent-Type: {}\r\n\r\n{}\r\n",
											 response_map[code], headers["Accept"], page);
	}

	std::string html = templ.block_contents[page];
	templ.prep_html(html);

	return std::format("HTTP/1.1 {}\r\nContent-Type: {}\r\n\r\n{}\r\n",
										 response_map[code], headers["Accept"], html);
}

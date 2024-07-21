#include "webserve.h"

#include <asm-generic/socket.h>
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

#include "WebContext.h"

#define SA struct sockaddr

webserve::webserve(std::string pages, int port)
		: port(port), pages(pages), templ(pages)
{
	int conn_fd;
	struct sockaddr_in addr, cli;
	int opt = 1;
	int addrlen = sizeof(cli);

	// socket create
	if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	{
		perror("socket failed");
	}

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(port);

	// binding socket to given IP and verification
	if ((bind(server_socket, (SA *)&addr, sizeof(addr))) != 0)
	{
		perror("socket bind failed...\n");
	}
	else
		printf("Socket bind successful.... \n");
}

webserve::~webserve() { close(server_socket); }

void webserve::start()
{
	running = true;
	std::thread listen_thread([this]() { listen_loop(); });

	listen_thread.join();
}

std::vector<std::string> webserve::split_string(std::string str,
																								const std::string &delim)
{
	std::vector<std::string> tokens;
	std::string token;

	size_t pos = 0;
	size_t start = 0;

	while ((pos = str.find(delim, start)) != std::string::npos)
	{
		token = str.substr(start, pos - start);

		start = pos + delim.length();
		tokens.push_back(token);
	}

	token = str.substr(start, str.length() - start);
	tokens.push_back(token);

	return tokens;
}

bool webserve::contains(std::string str, std::string token)
{
	return str.find(token) != std::string::npos;
}

void webserve::stop() { running = false; }

// TODO: this doesn't get all headers
void webserve::add_headers(std::map<std::string, std::string> &headers,
													 std::vector<std::string> lines)
{
	for (int i = 1; i < lines.size(); i++)
	{
		std::vector<std::string> header_line = split_string(lines[i], ":");
		if (header_line.size() < 2)
		{
			continue;
		}
		headers[header_line[0]] = header_line[1];
	}
}

bool webserve::isPath(std::string path)
{
	return (get_map.contains(path) || post_map.contains(path));
}

std::string webserve::send_file(std::string path, WebContext context)
{
	std::string page = templ.load_file(path);
	std::string ret_code = "200 OK";
	if (page == "")
	{
		ret_code = "500 Internal Server Error";
	}

	return std::format("HTTP/1.1 {}\r\nContent-Type: {}\r\n\r\n{}\r\n", ret_code,
										 context.headers["Accept"], page);
}

void webserve::handle_client(int client_fd)
{
	std::string response;
	char request[1024] = {0};
	int valread = read(client_fd, request, 1024);

	WebContext context{templ};
	context.client_fd = client_fd;

	std::vector<std::string> lines = split_string(request, "\r\n");
	std::vector<std::string> request_line = split_string(lines[0], " ");

	add_headers(context.headers, lines);

	if (tmpp::isFile(templ.public_dir + request_line[1]) &&
			!isPath(request_line[1]))
	{
		response = send_file(templ.public_dir + request_line[1], context);
		send(client_fd, response.c_str(), response.length(), 0);
		close(client_fd);
		return;
	}

	if (!isPath(request_line[1]))
	{
		response = std::format("HTTP/1.1 {}\r\nContent-Type: {}\r\n\r\n{}\r\n",
													 "404 Not Found", context.headers["Accept"],
													 "PAGE NOT FOUND");

		send(client_fd, response.c_str(), response.length(), 0);
		close(client_fd);
		return;
	}

	if (request_line[0] == "GET" && get_map.contains(request_line[1]))
	{
		response = get_map[request_line[1]](context);
	}
	else if (request_line[0] == "POST" && post_map.contains(request_line[1]))
	{
		response = post_map[request_line[1]](context);
	}

	send(client_fd, response.c_str(), response.length(), 0);
	close(client_fd);
}

void webserve::listen_loop()
{
	struct sockaddr_in addr;
	int addrlen = sizeof(addr);

	if ((listen(server_socket, 5)) != 0)
	{
		perror("Listen Failed....");
	}

	while (running)
	{
		int conn_fd = accept(server_socket, (SA *)&addr, (socklen_t *)&addrlen);

		std::thread client_thread([this, conn_fd]() { handle_client(conn_fd); });
		client_thread.detach();
	}
}

void webserve::GET(std::string path, std::function<std::string(WebContext)> cb)
{
	get_map[path] = cb;
}

void webserve::POST(std::string path, std::function<std::string(WebContext)> cb)
{
	post_map[path] = cb;
}

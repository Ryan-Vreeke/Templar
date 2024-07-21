#include <stdlib.h>
#include <sys/socket.h>

#include <csignal>
#include <cstdio>
#include <cstring>
#include <string>

#include "webserve.h"

webserve *web;

void signalHandler(int signal)
{
	if (signal == SIGINT)
	{
		std::cout << "Closing Server" << std::endl;
		web->stop();

		delete web;
	}
}

int main(int argc, char *argv[])
{
	std::signal(SIGINT, signalHandler);
	int port;
	std::cin >> port;

	std::cout << "Port: " << port << std::endl;
	web = new webserve{"./public", port};

	web->GET("/", [](WebContext ctx) -> std::string	{ 
    return ctx.Render(200, "index"); 
  });

	web->start();
	return 0;
}

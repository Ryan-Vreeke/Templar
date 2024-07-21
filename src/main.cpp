
#include <sys/socket.h>
#include <unistd.h>

#include <csignal>
#include <cstdio>
#include <cstring>
#include "webserve.h"

webserve *web;
using namespace std;


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
	WatchDirectory("./public");
	// std::signal(SIGINT, signalHandler);
	// int port;
	// std::cin >> port;

	// std::cout << "Port: " << port << std::endl;
	// web = new webserve{"./public", port};

	// web->GET("/", [](WebContext ctx) -> std::string	{
	//    return ctx.Render(200, "index");
	//  });

	// web->start();
	//

	return 0;
}

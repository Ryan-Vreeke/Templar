
#include <sys/socket.h>
#include <unistd.h>

#include <csignal>
#include <cstdio>
#include <cstring>

#include "Watchman.h"
#include "webserve.h"

webserve* web;
Watchman w("./public");
using namespace std;

void signalHandler(int signal)
{
	if (signal == SIGINT)
	{
		std::cout << "Closing Server" << std::endl;
		// web->stop();
    w.stop();

		delete web;
	}
}

void created(const std::string& str)
{
}

void deleted_cb(const std::string& str)
{
}

void modifi(const std::string& str)
{
}

void move_in(const std::string& str)
{
}

void move_out(const std::string& str)
{
}

int main(int argc, char* argv[])
{
	// watchman_cb cb;
	// cb.created_cb = created;
	// cb.delete_cb = deleted_cb;
	// cb.modify_cb = modifi;
	// cb.move_in_cb = move_in;
	// cb.move_out_cb = move_out;

	// w.init_watchman();
	// w.start_watch(&cb);

	// std::signal(SIGINT, signalHandler);
	// int port;
	// std::cin >> port;

	// std::cout << "Port: " << port << std::endl;
	// web = new webserve{"./public", port};

	web->GET("/", [](WebContext ctx) -> std::string	{
	   return ctx.Render(200, "index");
	 });

	web->start();
	//

	return 0;
}

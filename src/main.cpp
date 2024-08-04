
#include <sys/socket.h>
#include <unistd.h>

#include <csignal>
#include <cstdio>
#include <cstring>

#include "WebContext.h"
#include "webserve.h"

webserve* web;
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

typedef struct
{
	int w;
	int h;
} Home;

int main(int argc, char* argv[])
{
	int port;
	cin >> port;

	web = new webserve{"./public", port};

	web->GET("/", [](WebContext ctx) -> std::string	{
	   std::map<std::string, std::string> h = {
	     {".Title", "Test"},
	     {"Sub", "substitute"},
	   };

	   return ctx.Render(200, "index", h);
	});

	// web->POST("/home", [](WebContext ctx) -> std::string{
	//   Home h{0,0};
	//   cout << ctx.body << endl;
	//   return ctx.Render(200, "home", h);
	// });

	// web->POST("/projects", [](WebContext ctx) -> std::string
	// {
	//   Home h{0,0};
	//   cout << ctx.body << endl;
	//   return ctx.Render(200, "projects", h);
	// });

	// web->POST("/work", [](WebContext ctx) -> std::string{
	//   Home h{0,0};
	//   cout << ctx.body << endl;
	//   return ctx.Render(200, "work", h);
	// });

	web->start();
	return 0;
}

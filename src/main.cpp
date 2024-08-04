
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

int main(int argc, char* argv[])
{
	tmpp t{"./public"};
	auto html = t.load_file("./public/index.html");
	auto fors = t.find_for(html);

	for (auto f : fors)
	{
		cout << t.get_for_content(html, f) << "\n";
		cout << t.for_iterations(html, f) << "\n";
	}

	// int port;
	// cin >> port;

	// web = new webserve{"./public", port};

	// web->GET("/", [](WebContext ctx) -> std::string	{
	//    std::map<std::string, std::string> h = {
	//      {".Title", "Test"},
	//      {"Sub", "substitute"},
	//    };

	//    return ctx.Render(200, "index", h);
	// });

	// web->start();
	return 0;
}

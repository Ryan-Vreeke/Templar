#pragma once

#include <limits.h>
#include <stdlib.h>
#include <sys/inotify.h>
#include <sys/socket.h>
#include <unistd.h>

#include <csignal>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

typedef void (*callback)(const std::string &);

typedef struct
{
	callback modify_cb;
	callback delete_cb;
	callback created_cb;
	callback move_in_cb;
	callback move_out_cb;
} watchman_cb;

class Watchman
{
 public:
	Watchman(const std::string &directory);
	Watchman(Watchman &&) = default;
	Watchman(const Watchman &) = default;
	Watchman &operator=(Watchman &&) = default;
	Watchman &operator=(const Watchman &) = default;
	~Watchman();

	int init_watchman();
	void start_watch(watchman_cb *cb);

 private:
	const std::string &_directory;
	int inotifyFd;
};

Watchman::Watchman(const std::string &directory) : _directory(directory) {}

Watchman::~Watchman() {}

int Watchman::init_watchman()
{
	inotifyFd = inotify_init();
	if (inotifyFd < 0)
	{
		perror("inotify_init");
		return -1;
	}

	int wd = inotify_add_watch(
			inotifyFd, _directory.c_str(),
			IN_DELETE | IN_MOVED_TO | IN_MOVED_FROM | IN_MODIFY | IN_CREATE);

	if (wd < 0)
	{
		perror("inotify_add_watch");
		close(inotifyFd);
		return -2;
	}

	return 0;
}

void Watchman::start_watch(watchman_cb *cb)
{
	std::vector<char> buffer(
			(size_t)(10 * (sizeof(struct inotify_event) + NAME_MAX + 1)));
	while (true)
	{
		ssize_t length = read(inotifyFd, buffer.data(), buffer.size());
		if (length < 0)
		{
			perror("read");
			close(inotifyFd);
			return;
		}

		ssize_t i = 0;
		while (i < length)
		{
			struct inotify_event *event = (struct inotify_event *)&buffer[i];
			if (event->len)
			{
				if (event->mask & IN_MODIFY)
				{
					cb->modify_cb(event->name);
					std::cout << "File modified: " << event->name << std::endl;
				}
				else if (event->mask & IN_CREATE)
				{
					cb->created_cb(event->name);
					std::cout << "File created: " << event->name << std::endl;
				}
				else if (event->mask & IN_DELETE)
				{
					cb->delete_cb(event->name);
					std::cout << "File deleted: " << event->name << std::endl;
				}
				else if (event->mask & IN_MOVED_TO)
				{
					cb->move_in_cb(event->name);
					std::cout << "File moved into directory: " << event->name
										<< std::endl;
				}
				else if (event->mask & IN_MOVED_FROM)
				{
					cb->move_out_cb(event->name);
					std::cout << "File moved out of directory: " << event->name
										<< std::endl;
				}
			}
			i += sizeof(struct inotify_event) + event->len;
		}
	}

	close(inotifyFd);
}

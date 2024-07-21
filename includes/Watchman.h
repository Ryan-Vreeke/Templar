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

  bool watch = true;

	Watchman(const std::string &directory);
	Watchman(Watchman &&) = default;
	Watchman(const Watchman &) = default;
	Watchman &operator=(Watchman &&) = default;
	Watchman &operator=(const Watchman &) = default;
	~Watchman();

	int init_watchman();
	void start_watch(watchman_cb *cb);
  void stop();

 private:
	const std::string &_directory;
	int inotifyFd;

  void run_watchman(watchman_cb *cb);

};


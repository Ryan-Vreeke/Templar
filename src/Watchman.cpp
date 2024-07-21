#include "Watchman.h"

#include <thread>

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

void Watchman::run_watchman(watchman_cb *cb)
{
	std::vector<char> buffer( (size_t)(10 * (sizeof(struct inotify_event) + NAME_MAX + 1)));
	while (watch)
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
				}
				else if (event->mask & IN_CREATE)
				{
					cb->created_cb(event->name);
				}
				else if (event->mask & IN_DELETE)
				{
					cb->delete_cb(event->name);
				}
				else if (event->mask & IN_MOVED_TO)
				{
					cb->move_in_cb(event->name);
				}
				else if (event->mask & IN_MOVED_FROM)
				{
					cb->move_out_cb(event->name);
				}
			}
			i += sizeof(struct inotify_event) + event->len;
		}
	}

	close(inotifyFd);
}

void Watchman::start_watch(watchman_cb *cb)
{
	std::thread watchman_thread([this, cb]() { run_watchman(cb); });
  watchman_thread.detach();
}

void Watchman::stop(){
  watch = false;
}


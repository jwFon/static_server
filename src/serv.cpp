// linux
// socket setsockopt bind listen accept
#include <sys/socket.h>
// struct sockaddr_in
#include <netinet/in.h>
// htonl
#include <arpa/inet.h>
// epoll_create1 struct epoll_event epoll_ctl epoll_wait
#include <sys/epoll.h>
// read write close
#include <unistd.h>
// fcntl
#include <fcntl.h>

// stdc
// memset
#include <cstring>
// perror
#include <cstdio>
// errno
#include <cerrno>

// cout
#include <iostream>
#include <string>
#include <vector>

#include "http.h"
#include "event.h"
#include "conf.h"


int main() {
	conf_init();
	
	int listen_fd = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in serv_addr;
	std::memset(&serv_addr, 0, sizeof(struct sockaddr_in));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(PORT);

	int on = 1;
	setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int));
	bind(listen_fd, (struct sockaddr*)&serv_addr, sizeof(struct sockaddr_in));

	// max of establishd but not accept connection
	listen(listen_fd, MAX_ESTABLISHED_COUNT);

	int epoll_fd = epoll_create1(0);
	struct epoll_event ev, events[MAX_EVENT];
	// add accept event
	event::accept_event ac_ev(listen_fd, epoll_fd);
	ev.data.ptr = &ac_ev;
	ev.events = EPOLLIN;
	epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_fd, &ev);

	
	while (true) {
		int event_cnt = epoll_wait(epoll_fd, events, MAX_EVENT, -1);
		if (event_cnt <= 0) {
			std::perror("epoll_wait");
			continue;
		}
		for (int i = 0; i < event_cnt; i++) {
			event::event *ready_event = static_cast<event::event*>(events[i].data.ptr);
			ready_event->deal();
		}
	}
}
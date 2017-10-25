#include <sys/socket.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <errno.h>

#include <cstdio>
#include <string>

#include "event.h"
#include "conf.h"
#include "http.h"

namespace event
{
accept_event::accept_event(int listen_fd, int epoll_fd) : _listen_fd(listen_fd), _epoll_fd(epoll_fd){}
void accept_event::deal() 
{	
	int conn_fd = accept(_listen_fd, NULL, NULL);

	int flags = fcntl(conn_fd, F_GETFL, 0);
	fcntl(conn_fd, F_SETFL, flags | O_NONBLOCK);

	read_req_event *read_event = new read_req_event(_epoll_fd, conn_fd);

	epoll_event ev;
	ev.events = EPOLLIN;
	ev.data.ptr = read_event;
	epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, conn_fd, &ev);
}
read_req_event::read_req_event(int epoll_fd, int conn_fd) : _epoll_fd(epoll_fd),  _conn_fd(conn_fd) {}
void read_req_event::deal()
{
	// store http request(include head and body) into the buffer
	std::string buffer;
	char block_buffer[BUF_BLOCK_SIZE];
	int readn ;

	while ((readn = read(_conn_fd, block_buffer, BUF_BLOCK_SIZE)) > 0 ) {
		buffer.append(block_buffer, readn);
	}
	if (readn == 0) {
		close(_conn_fd);
		epoll_event ev;
		epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, _conn_fd, &ev);
		delete this;
		return ;
	} else {
		if (errno == EWOULDBLOCK) {
			errno = 0;
		} else {
			std::perror("read");
		}
	}


	// parse buffer and get http::reqest;
	http::request req = http::parse(buffer);

	http::response resp(req.get_protocol());

	for (const auto handler : http::req_handlers) {
		if (!handler->do_req(req, resp) ) {
			break;
		}
	}

	resp.send(_conn_fd);
	close(_conn_fd);
	delete this;
}
}	// namespace event
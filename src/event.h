#ifndef _EVENT_H_
#define _EVENT_H_

namespace event
{
class event 
{
public:
	virtual void deal(){}
	virtual ~event() = default;
};	// class event
// to accept the new connection
class accept_event : public event {
public:
	accept_event(int listen_fd, int epoll_fd);
	virtual void deal() override;
	~accept_event() {}
private:
	int _listen_fd;
	int _epoll_fd;
};	// accept_event
// to read the tcp message into the buffer and deal the request
class  read_req_event : public event {
public:
	read_req_event(int epoll_fd, int conn_fd);
	virtual void deal() override;
	~read_req_event() {}
private:
	int _epoll_fd;
	int _conn_fd;
};
}	// namespace ev

#endif
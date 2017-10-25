#ifndef _HTTP_H_
#define _HTTP_H_
#include <string>
#include <iostream>
#include <vector>
#include <unordered_map>

namespace http {

class request {
friend request parse(std::string& req_buffer);
public:
	std::string get_protocol() { return protocol;}
	std::string get_uri_name() const {
		auto begin = uri.find_first_of("/");
		auto end= uri.find_last_of("?");
		return uri.substr(begin, end - begin);
	}
private:
	//  Request-Line   = Method SP Request-URI SP HTTP-Version CRLF
	std::string method;
	std::string uri;
	std::string protocol;
	// headers;
	std::unordered_map<std::string, std::string> headers;
	// message-body
	std::string body;
};	// class request
request parse(std::string& req_buffer);

const std::unordered_map<int, std::string> code_msgs = {
	{200, "OK"},
	{403, "Forbidden"},
	{404, "Not Found"},
	{500, "Internal Server Error"}
};

class response;
class handler {
public:
	virtual bool do_req(const request& req, response& resp) = 0;
};

class info_handler : public handler {
public:
	bool do_req(const request& req, response& resp) override;
};

class static_file_handler : public handler {
public:
	static_file_handler(const std::string& root_dir) : root_dir(root_dir){}
	bool do_req(const request& req, response& resp) override;
private:
	std::string root_dir;
};

extern std::vector<http::handler*> req_handlers;

class response {
friend bool handler::do_req(const request& req, response& resp);
friend bool info_handler::do_req(const request& req, response& resp);
friend bool static_file_handler::do_req(const request& req, response& resp);
public:
	response(std::string protocol) : protocol(protocol), status(200) {}
	bool send(int fd);
private:
	std::string protocol;
	int status;
	std::vector<std::string> header_keys;
	std::unordered_map<std::string, std::string> headers;
	std::string body;
};



}	// namespace http
#endif
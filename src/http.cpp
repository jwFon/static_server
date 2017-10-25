#include <iostream>
#include <sstream>
#include <string>

#include <ctime>
#include <cstdio>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "http.h"
#include "conf.h"

namespace http {

std::vector<http::handler*> req_handlers = {
	new http::info_handler(),
	new http::static_file_handler("../html")
};

request parse(std::string& req_buffer)
{
	request req;
	std::istringstream req_in(req_buffer);
	std::string line;
	//  Request-Line   = Method SP Request-URI SP HTTP-Version CRLF
	// parse request line
	std::getline(req_in, line);
	std::istringstream req_line_stream(line);
	std::getline(req_line_stream, req.method, ' ');
	std::getline(req_line_stream, req.uri, ' ');
	std::getline(req_line_stream, req.protocol, ' ');

	// parse headers
	std::string header_key, header_val;
	while (std::getline(req_in, line) && line != "\r") {
		std::istringstream header_line_stream(line);
		std::getline(header_line_stream, header_key, ':');
		std::getline(header_line_stream, header_val);
		req.headers[header_key] = header_val;
	}
	// parse message body
	req_in >> req.body;
	return req;
}

bool response::send(int fd)
{
	std::string header;
	std::string tmp_pro = "HTTP/1.1";
	header = tmp_pro + " " +  std::to_string(status) + " " +  code_msgs.at(status) + "\r\n";

	for (const auto& h_key : header_keys) {
		header += h_key + ": " + headers[h_key] + "\r\n";
	}
	header += "\r\n";

	int res = write(fd, header.c_str(), header.size());
	if (res < 0) {
		std::perror("write header");
	}
	res = write(fd, body.c_str(), body.size());
	if (res < 0) {
		std::perror("write body");
	}
	return true;
}

bool info_handler::do_req(const request& req, response& resp) 
{
	std::time_t now = std::time(0);
	std::tm* now_tm = std::localtime(&now);
	char time_str[30] = {0};
	std::strftime(time_str, 30, "%a, %d %b %Y %H:%M:%S GMT", now_tm);
	resp.header_keys.push_back("Date");
	resp.headers["Date"] = std::string(time_str);

	resp.header_keys.push_back("Server");
	resp.headers["Server"] = "Jw-server/0.1";

	return true;
}

// get the static file and put it to the resp.body
bool static_file_handler::do_req(const request& req, response& resp)
{	
	std::string file_name = req.get_uri_name();
	std::string file_path;

	if (file_name == "/") {
		file_name = "/index.html";
	}
	file_path = root_dir + file_name;

	int fd = open(file_path.c_str(), O_RDONLY);
	if (fd < 0) {
		resp.status = 404;
		file_path = root_dir + "/404.html";
		fd = open(file_path.c_str(), O_RDONLY);
	}

	std::string& buffer = resp.body;
	char block_buffer[1024];
	int readn ;
	while ((readn = read(fd, block_buffer, 1024)) > 0 ) {
		buffer.append(block_buffer, readn);
	}
	close(fd);

	return true;
}
}	// namespace http
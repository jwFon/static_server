A static HTTP server
====

a HTTP server implementation used by epoll

## Introduction

This is a server that parses an HTTP message request and then find the resource(a static file in the configured directory)  then responses it to the HTTP client. If not found, return the familiar 404 Not Found status. 
The accept event and read event are driven by epoll, so the main thread will not block when accepting or reading, which brings higher performance. Epoll are widely used in high performance server.

## Compile and run (only support Linux2.6+)
```
cd src
make
./serv
```

## Thanks
Thanks for google and Linux.
And thank you for your reading.
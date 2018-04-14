#pragma once

#include <winsock2.h>
#include <Ws2tcpip.h>
#include <windows.h>
#pragma comment(lib, "ws2_32")

#include <string>
#include <iostream>
#include <sstream>
using std::string;
using std::cout;
using std::endl;
using std::ostringstream;




class TCP_server
{
public:
	TCP_server(void);
	~TCP_server(void);
	bool init(const long unsigned int src_port_number);
	bool check_for_pending_connection(void);
	int send_data(const char *buf, int len, int flags);
	int recv_data(char *buf, int len, int flags);
	void close(void);

protected:
	SOCKET tcp_socket, accept_socket;
	long unsigned int port_number;
};

class TCP_client
{
public:
	TCP_client(void);
	~TCP_client(void);
	bool init(const string src_hostname, const long unsigned int src_port_number);
	int send_data(const char *buf, int len, int flags);
	int recv_data(char *buf, int len, int flags);
	void close(void);

protected:
	SOCKET tcp_socket;
	string hostname;
	long unsigned int port_number;
};


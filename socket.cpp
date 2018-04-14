#include "socket.h"

TCP_server::TCP_server(void)
{
	tcp_socket = INVALID_SOCKET;
}

TCP_server::~TCP_server(void)
{
	close();
}

bool TCP_server::init(const long unsigned int src_port_number)
{
	close();

	port_number = src_port_number;

	struct sockaddr_in my_addr;
	int sock_addr_len = sizeof(struct sockaddr);
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons((unsigned short int)port_number);
	my_addr.sin_addr.s_addr = INADDR_ANY;
	memset(&(my_addr.sin_zero), '\0', 8);

	if (INVALID_SOCKET == (tcp_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)))
	{
		cout << "  Could not allocate a new socket." << endl;
		return false;
	}

	if (SOCKET_ERROR == bind(tcp_socket, (struct sockaddr *) &my_addr, sizeof(struct sockaddr)))
	{
		cout << "  Could not bind socket to port " << port_number << "." << endl;
		return false;
	}

	if (SOCKET_ERROR == listen(tcp_socket, 0))
	{
		cout << "  Listen error." << endl;
		return false;
	}

	long unsigned int nb = 1;
	if (SOCKET_ERROR == ioctlsocket(tcp_socket, FIONBIO, &nb))
	{
		cout << "  Setting non-blocking mode failed." << endl;
		return false;
	}

	return true;
}

bool TCP_server::check_for_pending_connection(void)
{
	struct sockaddr_in my_addr;
	int sock_addr_len = sizeof(struct sockaddr);
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons((unsigned short int)port_number);
	my_addr.sin_addr.s_addr = INADDR_ANY;
	memset(&(my_addr.sin_zero), '\0', 8);

	if (INVALID_SOCKET == (accept_socket = accept(tcp_socket, (struct sockaddr *) &my_addr, &sock_addr_len)))
	{
		if (WSAEWOULDBLOCK != WSAGetLastError())
			cout << "  Accept error." << endl;

		return false;
	}

	return true;
}

int TCP_server::send_data(const char *buf, int len, int flags)
{
	return send(accept_socket, buf, len, flags);
}

int TCP_server::recv_data(char *buf, int len, int flags)
{
	return recv(accept_socket, buf, len, flags);
}

void TCP_server::close(void)
{
	if (INVALID_SOCKET != accept_socket)
		closesocket(accept_socket);

	if (INVALID_SOCKET != tcp_socket)
		closesocket(tcp_socket);

	port_number = 0;
}

TCP_client::TCP_client(void)
{
	tcp_socket = INVALID_SOCKET;
}

TCP_client::~TCP_client(void)
{
	close();
}

bool TCP_client::init(const string src_hostname, const long unsigned int src_port_number)
{
	close();

	hostname = src_hostname;
	port_number = src_port_number;

	struct addrinfo hints;
	struct addrinfo *result;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = 0;
	hints.ai_protocol = IPPROTO_TCP;

	ostringstream oss;
	oss << port_number;

	if (0 != getaddrinfo(hostname.c_str(), oss.str().c_str(), &hints, &result))
	{
		cout << "  getaddrinfo error " << WSAGetLastError() << endl;
		freeaddrinfo(result);
		return false;
	}

	if (INVALID_SOCKET == (tcp_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)))
	{
		cout << "  Could not allocate a new socket " << WSAGetLastError() << endl;
		freeaddrinfo(result);
		return false;
	}

	if (SOCKET_ERROR == connect(tcp_socket, (struct sockaddr *)result->ai_addr, sizeof(struct sockaddr)))
	{
		cout << "  Connect error " << WSAGetLastError() << endl;
		freeaddrinfo(result);
		return false;
	}

	long unsigned int nb = 1;
	if (SOCKET_ERROR == ioctlsocket(tcp_socket, FIONBIO, &nb))
	{
		cout << "  Setting non-blocking mode failed " << WSAGetLastError() << endl;
		freeaddrinfo(result);
		return false;
	}

	freeaddrinfo(result);
	return true;
}

int TCP_client::send_data(const char *buf, int len, int flags)
{
	return send(tcp_socket, buf, len, flags);
}

int TCP_client::recv_data(char *buf, int len, int flags)
{
	return recv(tcp_socket, buf, len, flags);
}

void TCP_client::close(void)
{
	if (INVALID_SOCKET != tcp_socket)
		closesocket(tcp_socket);

	port_number = 0;
	hostname = "";
}


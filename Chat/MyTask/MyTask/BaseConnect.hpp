#ifndef _BASE_CONNECT_HPP
#define _BASE_CONNECT_HPP

#include <WinSock2.h>
#include <fstream>
#pragma comment (lib, "ws2_32.lib")

class BaseConnect
{

public:

	BaseConnect( const unsigned int _messageLength );
	void removeConnection();

	bool isCorrectConnection() const;

protected:

	virtual bool sendMessage( const std::string& _message, const SOCKET _socket );
	virtual bool recvMessage( std::string& _receivedMessage, const SOCKET _socket );

protected:

	SOCKET m_socket;
	const unsigned int m_messageLength;
	char * m_recvBuffer;
	bool m_isCorrectConnection;

};

#endif // _BASE_CONNECT_HPP
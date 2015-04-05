#ifndef _CLIENT_HPP
#define _CLIENT_HPP

#include "BaseConnect.hpp"
#include <string>

class Client
	:	public BaseConnect
{

public:

	Client( const std::string& _ipAddress, const unsigned int _messageLength = 1024 );
	bool reconnect( const std::string& _ipAddress );
	bool isClientConnectWithServer();
	/*virtual*/ bool sendMessage( const std::string& _message );
	/*virtual*/ bool recvMessage( std::string& _receivedMessage );

private:

	bool m_isClientConnectWithServer;
	
};

#endif // _CLIENT_HPP
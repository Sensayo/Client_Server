#ifndef _SERVER_HPP
#define _SERVER_HPP

#include "BaseConnect.hpp"
#include <map>
#include <set>

class Server
	:	public BaseConnect
{

public:

	Server( unsigned int _messageLength = 1024 );
	void removeConnection( );

	bool acceptNewSocket( );
	void recvMessage( SOCKET _socket );

	std::map< SOCKET, std::string >::const_iterator connectionsBegin() const;
	std::map< SOCKET, std::string >::const_iterator connectionsEnd() const;

	void blackListAdd( std::string _ip );
	void blackListErase( std::string _ip );
	void blackListClear();

private:

	std::set< std::string > m_blackList;
	std::map< SOCKET, std::string > m_connectionsMap;

};

#endif // _SERVER_HPP
#include "Server.hpp"
#include <algorithm>
#include <thread>

Server::Server( unsigned int _messageLength )
	:	BaseConnect( _messageLength )
{
	if ( !m_isCorrectConnection )
		return;

	sockaddr_in addr;
	ZeroMemory( &addr, sizeof ( addr ) );
	addr.sin_addr.s_addr = htonl( INADDR_ANY );
	addr.sin_port = htons( 1995 );
	addr.sin_family = AF_INET;

	int iResult = bind( m_socket, ( sockaddr* ) &addr, sizeof( addr ) );
	if ( iResult == SOCKET_ERROR )
	{
		m_isCorrectConnection = false;
		BaseConnect::removeConnection();
		return;
	}

	iResult = listen( m_socket, SOMAXCONN );
	if ( iResult == SOCKET_ERROR )
	{
		m_isCorrectConnection = false;
		BaseConnect::removeConnection();
		return;
	}
}

void Server::removeConnection()
{
	BaseConnect::removeConnection();

	std::for_each( m_connectionsMap.begin(), m_connectionsMap.end(), [] ( std::pair< SOCKET, std::string > _pair )
	{
		closesocket( _pair.first );
	} );
}

void Server::recvMessage( SOCKET _socket )
{
	static std::string messsage;

	while ( BaseConnect::recvMessage( messsage, _socket ) )
	{
		auto it = std::find( m_blackList.begin(), m_blackList.end(), m_connectionsMap[ _socket ] );

		if ( it != m_blackList.end() )
			continue;

		std::for_each( m_connectionsMap.begin( ), m_connectionsMap.end( ), [&] ( std::pair< SOCKET, std::string > _pair )
		{
			BaseConnect::sendMessage( messsage, _pair.first );
		} );
	}
}

bool Server::acceptNewSocket()
{
	if ( !m_isCorrectConnection )
		return false;

	SOCKET newSocket;
	sockaddr_in newAddres;
	int size = sizeof( newAddres );
	
	newSocket = accept( m_socket, ( sockaddr* ) &newAddres, &size );
	if ( newSocket == INVALID_SOCKET )
		return false;
	
	std::string ipAddres = inet_ntoa( newAddres.sin_addr );
	m_connectionsMap.insert( std::make_pair( newSocket, ipAddres ) );

	std::thread thread( &Server::recvMessage, this, newSocket );
	thread.detach();

	return true;
}

std::map< SOCKET, std::string >::const_iterator Server::connectionsBegin() const
{
	return m_connectionsMap.cbegin();
}

std::map< SOCKET, std::string >::const_iterator Server::connectionsEnd() const
{
	return m_connectionsMap.cend( );
}

void Server::blackListAdd( std::string _ip )
{
	m_blackList.insert( _ip );
}

void Server::blackListErase( std::string _ip )
{
	m_blackList.erase( _ip );
}

void Server::blackListClear()
{
	m_blackList.clear();
}
#include "Client.hpp"

Client::Client( const std::string& _ipAddress, const unsigned int _messageLength )
	:	BaseConnect( _messageLength ), m_isClientConnectWithServer( false )
{
	if ( !m_isCorrectConnection )
		return;

	sockaddr_in addr;
	ZeroMemory( &addr, sizeof ( addr ) );
	addr.sin_addr.S_un.S_addr = inet_addr( _ipAddress.c_str( ) );
	addr.sin_port = htons( 1995 );
	addr.sin_family = AF_INET;

	int iResult = connect( m_socket, ( sockaddr* ) &addr, sizeof( addr ) );
	if ( iResult == SOCKET_ERROR )
		m_isClientConnectWithServer = false;
	else
		m_isClientConnectWithServer = true;
}

bool Client::reconnect( const std::string& _ipAddress )
{
	if ( !m_isCorrectConnection )
		return false;

	sockaddr_in addr;
	ZeroMemory( &addr, sizeof ( addr ) );
	addr.sin_addr.S_un.S_addr = inet_addr( _ipAddress.c_str( ) );
	addr.sin_port = htons( 1995 );
	addr.sin_family = AF_INET;

	int iResult = connect( m_socket, ( sockaddr* ) &addr, sizeof( addr ) );
	if ( iResult == SOCKET_ERROR )
		m_isClientConnectWithServer = false;
	else
		m_isClientConnectWithServer = true;

	return m_isClientConnectWithServer;
}

bool Client::sendMessage( const std::string& _message )
{
	return BaseConnect::sendMessage( _message, m_socket );
}

bool Client::recvMessage( std::string& _receivedMessage )
{
	if ( !BaseConnect::recvMessage( _receivedMessage, m_socket ) )
		m_isClientConnectWithServer = false;

	return m_isClientConnectWithServer;
}

bool Client::isClientConnectWithServer()
{
	return m_isClientConnectWithServer;
}
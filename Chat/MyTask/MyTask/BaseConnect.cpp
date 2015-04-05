#include "BaseConnect.hpp"

BaseConnect::BaseConnect( const unsigned int _messageLength )
:			m_messageLength( _messageLength )
		,	m_isCorrectConnection( true )
		,	m_recvBuffer( nullptr )
		,	m_socket( INVALID_SOCKET )
{
	WSADATA wsaData;
	int iResult = WSAStartup( MAKEWORD( 2, 2 ), &wsaData );
	if ( iResult != 0 )
	{
		m_isCorrectConnection = false;
		return;
	}

	m_socket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
	if ( m_socket == INVALID_SOCKET )
	{
		m_isCorrectConnection = false;
		removeConnection();
		return;
	}

	m_recvBuffer = new char[ m_messageLength ];
}

void BaseConnect::removeConnection()
{
	WSACleanup();
	closesocket( m_socket );

	if ( m_recvBuffer )
		delete[] m_recvBuffer;
}

bool BaseConnect::sendMessage( const std::string& _message, const SOCKET _socket )
{
	if ( !m_isCorrectConnection )
		return false;

	int iResult = send( _socket, _message.c_str( ), m_messageLength, 0 );
	if ( iResult == SOCKET_ERROR )
		return false;

	return true;
}

bool BaseConnect::recvMessage( std::string& _receivedMessage, const SOCKET _socket )
{
	if ( !m_isCorrectConnection )
		return false;

	int iResult = recv( _socket, m_recvBuffer, m_messageLength, 0 );

	if ( iResult > 0 )
	{
		_receivedMessage = m_recvBuffer;
		return true;
	}

	return false;
}

bool BaseConnect::isCorrectConnection() const
{
	return m_isCorrectConnection;
}